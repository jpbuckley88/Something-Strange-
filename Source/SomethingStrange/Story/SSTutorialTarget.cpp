#include "Story/SSTutorialTarget.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Entities/SSBodyPartComponent.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FName TutorialTargetTag(TEXT("Target"));
}

ASSTutorialTarget::ASSTutorialTarget()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetStaticMesh(CubeMesh.Object);
	Mesh->SetRelativeScale3D(FVector(0.3f));
	Mesh->ComponentTags.Add(TutorialTargetTag);

	BodyParts = CreateDefaultSubobject<USSBodyPartComponent>(TEXT("BodyParts"));
	BodyParts->bTintPlaceholderMeshes = true;

	// One part covering the whole target. Destroying it destroys the actor.
	FSSBodyPartState Part;
	Part.PartName = TutorialTargetTag;
	Part.ComponentTag = TutorialTargetTag;
	Part.bIsCore = true;
	BodyParts->Parts.Add(Part);
}

void ASSTutorialTarget::BeginPlay()
{
	// Copy this instance's settings into the part before the component begins play and reads them.
	if (BodyParts->Parts.Num() > 0)
	{
		FSSBodyPartState& Part = BodyParts->Parts[0];
		Part.MergeAmount = bMerged ? 1.f : 0.f;
		Part.MaxHealth = Health;
		Part.bIndestructible = bIndestructible;
	}

	Super::BeginPlay();

	BodyParts->OnPartDamaged.AddDynamic(this, &ASSTutorialTarget::HandlePartDamaged);

	if (!bStartActive)
	{
		SetTargetActive(false);
	}
}

void ASSTutorialTarget::SetTargetActive(bool bActive)
{
	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);
}

void ASSTutorialTarget::HandlePartDamaged(FName PartName, ESSFrequency Frequency, bool bMatched, float BaseDamage)
{
	if (Tested.Contains(Frequency))
	{
		return;
	}

	// Each hit is one frame of beam, so frame time adds up to seconds under the beam.
	float& Seconds = Exposure.FindOrAdd(Frequency);
	Seconds += GetWorld()->GetDeltaSeconds();

	if (Seconds >= SecondsToTest)
	{
		Tested.Add(Frequency);
		OnFrequencyTested.Broadcast(this, Frequency);
	}
}
