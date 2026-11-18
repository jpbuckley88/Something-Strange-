#include "Weapons/SSLaserWeapon.h"

#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Entities/SSBodyPartComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

ASSLaserWeapon::ASSLaserWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASSLaserWeapon::StartFire()
{
	bWantsToFire = true;
}

void ASSLaserWeapon::StopFire()
{
	bWantsToFire = false;
}

void ASSLaserWeapon::CycleFrequency()
{
	const UEnum* FrequencyEnum = StaticEnum<ESSFrequency>();
	// NumEnums includes the hidden _MAX entry that UHT appends.
	const int32 FrequencyCount = FrequencyEnum->NumEnums() - 1;
	const int32 NextIndex = (static_cast<int32>(CurrentFrequency) + 1) % FrequencyCount;
	SetFrequency(static_cast<ESSFrequency>(NextIndex));
}

void ASSLaserWeapon::SetFrequency(ESSFrequency NewFrequency)
{
	if (NewFrequency == CurrentFrequency)
	{
		return;
	}

	CurrentFrequency = NewFrequency;
	OnFrequencyChanged.Broadcast(CurrentFrequency);
}

FLinearColor ASSLaserWeapon::GetBeamColor() const
{
	return CurrentFrequency == ESSFrequency::Material ? MaterialBeamColor : SpectralBeamColor;
}

void ASSLaserWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsBeamActive())
	{
		TimeSinceBeamStopped = 0.f;
		FireBeam(DeltaTime);

		Heat = FMath::Min(MaxHeat, Heat + HeatPerSecond * DeltaTime);
		if (Heat >= MaxHeat)
		{
			bOverheated = true;
			OnOverheatChanged.Broadcast(true);
		}
		return;
	}

	TimeSinceBeamStopped += DeltaTime;
	if (TimeSinceBeamStopped >= CoolingDelay)
	{
		Heat = FMath::Max(0.f, Heat - CoolingPerSecond * DeltaTime);
	}

	if (bOverheated && Heat <= MaxHeat * OverheatRecoverFraction)
	{
		bOverheated = false;
		OnOverheatChanged.Broadcast(false);
	}
}

void ASSLaserWeapon::FireBeam(float DeltaTime)
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const AController* Controller = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	if (!Controller)
	{
		return;
	}

	// Aim from the player's view so the beam lands on the crosshair.
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * Range;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SSLaserBeam), false, this);
	Params.AddIgnoredActor(OwnerPawn);

	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, ECC_Visibility, Params);

	if (bHit)
	{
		if (const AActor* HitActor = Hit.GetActor())
		{
			if (USSBodyPartComponent* BodyParts = HitActor->FindComponentByClass<USSBodyPartComponent>())
			{
				BodyParts->ApplyBeamDamage(Hit, CurrentFrequency, DamagePerSecond * DeltaTime);
			}
		}
	}

	if (bDrawDebugBeam)
	{
		const FVector BeamEnd = bHit ? Hit.ImpactPoint : TraceEnd;
		DrawDebugLine(GetWorld(), GetActorLocation(), BeamEnd, GetBeamColor().ToFColor(true), false, -1.f, 0, 2.f);
	}
}
