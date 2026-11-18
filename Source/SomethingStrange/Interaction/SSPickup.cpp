#include "Interaction/SSPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/SSHealthComponent.h"
#include "Player/SSInventoryComponent.h"
#include "Player/SSPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASSPickup::ASSPickup()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	TouchSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TouchSphere"));
	RootComponent = TouchSphere;
	TouchSphere->InitSphereRadius(60.f);
	TouchSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Blocks only the visibility channel, so it can be looked at but not tripped over.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(TouchSphere);
	Mesh->SetStaticMesh(SphereMesh.Object);
	Mesh->SetRelativeScale3D(FVector(0.25f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ASSPickup::BeginPlay()
{
	Super::BeginPlay();

	TouchSphere->OnComponentBeginOverlap.AddDynamic(this, &ASSPickup::HandleOverlap);

	// Placeholder colours until pickups have their own meshes.
	FLinearColor Color = FLinearColor::White;
	switch (Type)
	{
	case ESSPickupType::Goggles: Color = FLinearColor(0.1f, 0.9f, 1.f); break;
	case ESSPickupType::Laser:   Color = FLinearColor(1.f, 0.35f, 0.05f); break;
	case ESSPickupType::Residue: Color = FLinearColor(0.4f, 1.f, 0.2f); break;
	case ESSPickupType::Health:  Color = FLinearColor(0.9f, 0.05f, 0.05f); break;
	}

	if (UMaterialInstanceDynamic* Material = Mesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

bool ASSPickup::IsCollectedOnTouch() const
{
	return Type == ESSPickupType::Residue || Type == ESSPickupType::Health;
}

void ASSPickup::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsCollectedOnTouch())
	{
		if (ASSPlayerCharacter* Ned = Cast<ASSPlayerCharacter>(OtherActor))
		{
			Collect(Ned);
		}
	}
}

void ASSPickup::Interact_Implementation(APawn* InteractingPawn)
{
	if (ASSPlayerCharacter* Ned = Cast<ASSPlayerCharacter>(InteractingPawn))
	{
		Collect(Ned);
	}
}

FText ASSPickup::GetInteractionPrompt_Implementation() const
{
	switch (Type)
	{
	case ESSPickupType::Goggles: return INVTEXT("Take the goggles");
	case ESSPickupType::Laser:   return INVTEXT("Take the laser");
	case ESSPickupType::Residue: return INVTEXT("Collect residue");
	case ESSPickupType::Health:  return INVTEXT("Use first aid");
	}

	return FText::GetEmpty();
}

void ASSPickup::Collect(ASSPlayerCharacter* Ned)
{
	switch (Type)
	{
	case ESSPickupType::Goggles:
		Ned->GetInventory()->GiveGoggles();
		break;
	case ESSPickupType::Laser:
		Ned->GetInventory()->GiveLaser();
		break;
	case ESSPickupType::Residue:
		Ned->GetInventory()->AddResidue(Amount);
		break;
	case ESSPickupType::Health:
		Ned->GetHealthComponent()->Heal(static_cast<float>(Amount));
		break;
	}

	Destroy();
}
