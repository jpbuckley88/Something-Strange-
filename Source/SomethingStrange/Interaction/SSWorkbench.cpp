#include "Interaction/SSWorkbench.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SSInventoryComponent.h"
#include "Player/SSPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASSWorkbench::ASSWorkbench()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	// Placeholder bench: 150 wide, 60 deep, 90 high, origin at its centre.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetStaticMesh(CubeMesh.Object);
	Mesh->SetRelativeScale3D(FVector(0.6f, 1.5f, 0.9f));
}

void ASSWorkbench::Interact_Implementation(APawn* InteractingPawn)
{
	if (bAssembled)
	{
		return;
	}

	const ASSPlayerCharacter* Ned = Cast<ASSPlayerCharacter>(InteractingPawn);
	if (!Ned)
	{
		return;
	}

	USSInventoryComponent* Inventory = Ned->GetInventory();
	if (Inventory->CountItems(RequiredItems) < RequiredItems.Num())
	{
		return;
	}

	if (Result == ESSPickupType::Goggles)
	{
		Inventory->GiveGoggles();
	}
	else if (Result == ESSPickupType::Laser)
	{
		Inventory->GiveLaser();
	}

	bAssembled = true;
	OnAssembled.Broadcast(this);
}

FText ASSWorkbench::GetInteractionPrompt_Implementation() const
{
	if (bAssembled)
	{
		return FText::GetEmpty();
	}

	if (!RequiredItems.IsEmpty())
	{
		const ASSPlayerCharacter* Ned = Cast<ASSPlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
		const int32 Carried = Ned ? Ned->GetInventory()->CountItems(RequiredItems) : 0;
		if (Carried < RequiredItems.Num())
		{
			return FText::Format(INVTEXT("Missing parts ({0} of {1})"), FText::AsNumber(Carried), FText::AsNumber(RequiredItems.Num()));
		}
	}

	return ResultName.IsEmpty()
		? INVTEXT("Assemble")
		: FText::Format(INVTEXT("Assemble the {0}"), ResultName);
}
