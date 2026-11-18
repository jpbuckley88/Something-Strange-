#include "Entities/SSEntityCharacter.h"

#include "AIController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Entities/SSBodyPartComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interaction/SSPickup.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SSHealthComponent.h"
#include "Spectral/SSSpectralSubsystem.h"

namespace
{
	const float RepathInterval = 0.4f;
}

ASSEntityCharacter::ASSEntityCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->BrakingDecelerationFlying = 1000.f;

	BodyParts = CreateDefaultSubobject<USSBodyPartComponent>(TEXT("BodyParts"));
	// The entity handles its own banishing so it can drop residue first.
	BodyParts->bDestroyOwnerOnBanish = false;
}

void ASSEntityCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = MoveSpeed;
	Movement->MaxFlySpeed = MoveSpeed;
	if (bFlying)
	{
		Movement->SetMovementMode(MOVE_Flying);
	}

	BodyParts->OnBanished.AddDynamic(this, &ASSEntityCharacter::HandleBanished);

	// Do not attack on the first frame in range.
	TimeSinceAttack = 0.f;
}

UStaticMeshComponent* ASSEntityCharacter::AddPlaceholderPart(FName PartName, UStaticMesh* Shape, const FVector& Location,
	const FVector& Scale, float MaxHealth, bool bMerged, bool bIsCore)
{
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(PartName);
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetStaticMesh(Shape);
	Mesh->SetRelativeLocation(Location);
	Mesh->SetRelativeScale3D(Scale);
	Mesh->ComponentTags.Add(PartName);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	FSSBodyPartState Part;
	Part.PartName = PartName;
	Part.ComponentTag = PartName;
	Part.MaxHealth = MaxHealth;
	Part.MergeAmount = bMerged ? 1.f : 0.f;
	Part.bIsCore = bIsCore;
	BodyParts->Parts.Add(Part);
	BodyParts->bTintPlaceholderMeshes = true;

	return Mesh;
}

bool ASSEntityCharacter::CanAttack() const
{
	if (AttackPartName.IsNone())
	{
		return true;
	}

	const int32 PartIndex = BodyParts->FindPartIndexByName(AttackPartName);
	return PartIndex != INDEX_NONE && !BodyParts->Parts[PartIndex].bDestroyed;
}

void ASSEntityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceAttack += DeltaTime;
	TimeSinceRepath += DeltaTime;

	if (BodyParts->IsBanished())
	{
		return;
	}

	APawn* Target = UGameplayStatics::GetPlayerPawn(this, 0);
	USSHealthComponent* TargetHealth = Target ? Target->FindComponentByClass<USSHealthComponent>() : nullptr;
	if (!TargetHealth || TargetHealth->IsDead())
	{
		StopMoving();
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	if (Distance > AggroRange)
	{
		StopMoving();
		return;
	}

	if (bFreezeWhileGogglesOn)
	{
		const USSSpectralSubsystem* Spectral = GetWorld()->GetSubsystem<USSSpectralSubsystem>();
		if (Spectral && Spectral->IsSpectralVisionActive())
		{
			StopMoving();
			return;
		}
	}

	if (Distance > AttackRange)
	{
		MoveTowards(Target);
		return;
	}

	StopMoving();

	if (TimeSinceAttack >= AttackInterval && CanAttack())
	{
		TimeSinceAttack = 0.f;
		TargetHealth->ApplyDamage(AttackDamage);
	}
}

void ASSEntityCharacter::MoveTowards(APawn* Target)
{
	if (bFlying)
	{
		const FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		AddMovementInput(Direction, 1.f);
		return;
	}

	if (TimeSinceRepath >= RepathInterval)
	{
		TimeSinceRepath = 0.f;
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			AI->MoveToActor(Target, AttackRange * 0.5f);
		}
	}
}

void ASSEntityCharacter::StopMoving()
{
	if (bFlying)
	{
		return; // No input means it brakes to a hover.
	}

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}
}

void ASSEntityCharacter::HandleBanished()
{
	if (ResidueDrop > 0)
	{
		// Drop the residue to the floor so a wisp banished in mid-air leaves it within reach.
		FVector DropLocation = GetActorLocation();
		FHitResult Floor;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(SSResidueDrop), false, this);
		if (GetWorld()->LineTraceSingleByChannel(Floor, DropLocation, DropLocation - FVector(0.f, 0.f, 2000.f), ECC_WorldStatic, Params))
		{
			DropLocation = Floor.ImpactPoint + FVector(0.f, 0.f, 30.f);
		}

		// Deferred, so the amount is set before the pickup begins play.
		const FTransform SpawnTransform(DropLocation);
		if (ASSPickup* Residue = GetWorld()->SpawnActorDeferred<ASSPickup>(ASSPickup::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			Residue->Type = ESSPickupType::Residue;
			Residue->Amount = ResidueDrop;
			Residue->FinishSpawning(SpawnTransform);
		}
	}

	Destroy();
}
