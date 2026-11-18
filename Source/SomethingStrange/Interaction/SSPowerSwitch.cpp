#include "Interaction/SSPowerSwitch.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASSPowerSwitch::ASSPowerSwitch()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	RootComponent = SwitchMesh;
	SwitchMesh->SetStaticMesh(CubeMesh.Object);
	SwitchMesh->SetRelativeScale3D(FVector(0.1f, 0.3f, 0.4f));
}

void ASSPowerSwitch::Interact_Implementation(APawn* InteractingPawn)
{
	if (bPowerOn && bOneWay)
	{
		return;
	}

	SetPowerOn(!bPowerOn);
}

FText ASSPowerSwitch::GetInteractionPrompt_Implementation() const
{
	if (bPowerOn)
	{
		return bOneWay ? FText::GetEmpty() : INVTEXT("Switch power off");
	}

	return INVTEXT("Switch power on");
}

void ASSPowerSwitch::SetPowerOn(bool bNewPowerOn)
{
	if (bPowerOn == bNewPowerOn)
	{
		return;
	}

	bPowerOn = bNewPowerOn;
	OnPowerChanged.Broadcast(bPowerOn);
}
