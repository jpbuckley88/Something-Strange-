#include "Interaction/SSDoor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASSDoor::ASSDoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	Hinge = CreateDefaultSubobject<USceneComponent>(TEXT("Hinge"));
	RootComponent = Hinge;

	// The cube is 100 units across with a centred pivot: offset it so the hinge is on one edge.
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Hinge);
	DoorMesh->SetStaticMesh(CubeMesh.Object);
	DoorMesh->SetRelativeLocation(FVector(50.f, 0.f, 100.f));
	DoorMesh->SetRelativeScale3D(FVector(1.f, 0.1f, 2.f));
}

void ASSDoor::Interact_Implementation(APawn* InteractingPawn)
{
	if (!bLocked)
	{
		SetOpen(!bOpen);
	}
}

FText ASSDoor::GetInteractionPrompt_Implementation() const
{
	if (bLocked)
	{
		return INVTEXT("Locked");
	}

	return bOpen ? INVTEXT("Close") : INVTEXT("Open");
}

void ASSDoor::SetOpen(bool bNewOpen)
{
	bOpen = bNewOpen;
	SetActorTickEnabled(true);
}

void ASSDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TargetAngle = bOpen ? OpenAngle : 0.f;
	CurrentAngle = FMath::FInterpConstantTo(CurrentAngle, TargetAngle, DeltaTime, SwingSpeed);
	Hinge->SetRelativeRotation(FRotator(0.f, CurrentAngle, 0.f));

	if (FMath::IsNearlyEqual(CurrentAngle, TargetAngle))
	{
		SetActorTickEnabled(false);
	}
}
