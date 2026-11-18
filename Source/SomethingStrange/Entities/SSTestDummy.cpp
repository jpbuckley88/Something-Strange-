#include "Entities/SSTestDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Entities/SSBodyPartComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FName ArmTag(TEXT("Arm"));
	const FName LegTag(TEXT("Leg"));
	const FName CoreTag(TEXT("Core"));

	// Same colours as the laser's beams, so a part's look says which frequency hurts it.
	const FLinearColor MergedColor(1.f, 0.35f, 0.05f);
	const FLinearColor UnmergedColor(0.1f, 0.9f, 1.f);
}

ASSTestDummy::ASSTestDummy()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Engine basic shapes are 100 units across with the pivot at the centre.
	Torso = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Torso"));
	RootComponent = Torso;
	Torso->SetStaticMesh(CubeMesh.Object);
	Torso->SetRelativeScale3D(FVector(0.4f, 0.6f, 1.f));

	// Children inherit the torso's scale, so their scales and offsets are relative to it.
	Arm = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arm"));
	Arm->SetupAttachment(Torso);
	Arm->SetStaticMesh(CubeMesh.Object);
	Arm->SetRelativeLocation(FVector(0.f, 75.f, 20.f));
	Arm->SetRelativeScale3D(FVector(0.6f, 0.4f, 0.7f));
	Arm->ComponentTags.Add(ArmTag);

	Leg = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Leg"));
	Leg->SetupAttachment(Torso);
	Leg->SetStaticMesh(CubeMesh.Object);
	Leg->SetRelativeLocation(FVector(0.f, -25.f, -85.f));
	Leg->SetRelativeScale3D(FVector(0.7f, 0.4f, 0.7f));
	Leg->ComponentTags.Add(LegTag);

	Core = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Core"));
	Core->SetupAttachment(Torso);
	Core->SetStaticMesh(SphereMesh.Object);
	Core->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	Core->SetRelativeScale3D(FVector(1.25f, 0.85f, 0.5f));
	Core->ComponentTags.Add(CoreTag);

	BodyParts = CreateDefaultSubobject<USSBodyPartComponent>(TEXT("BodyParts"));

	FSSBodyPartState ArmPart;
	ArmPart.PartName = ArmTag;
	ArmPart.ComponentTag = ArmTag;
	ArmPart.MergeAmount = 1.f;
	BodyParts->Parts.Add(ArmPart);

	FSSBodyPartState LegPart;
	LegPart.PartName = LegTag;
	LegPart.ComponentTag = LegTag;
	LegPart.MergeAmount = 0.f;
	LegPart.MergeRate = 0.025f; // Crosses over 20 seconds after play begins.
	BodyParts->Parts.Add(LegPart);

	FSSBodyPartState CorePart;
	CorePart.PartName = CoreTag;
	CorePart.ComponentTag = CoreTag;
	CorePart.MergeAmount = 0.f;
	CorePart.MaxHealth = 150.f;
	CorePart.bIsCore = true;
	BodyParts->Parts.Add(CorePart);
}

void ASSTestDummy::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInstanceDynamic* TorsoMaterial = Torso->CreateDynamicMaterialInstance(0))
	{
		TorsoMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.3f, 0.3f, 0.3f));
	}

	for (const FSSBodyPartState& Part : BodyParts->Parts)
	{
		SetPartColor(FindPartMesh(Part.PartName), Part.IsMerged());
	}

	BodyParts->OnPartMergeChanged.AddDynamic(this, &ASSTestDummy::HandlePartMergeChanged);
}

void ASSTestDummy::HandlePartMergeChanged(FName PartName, bool bMerged)
{
	SetPartColor(FindPartMesh(PartName), bMerged);
}

void ASSTestDummy::SetPartColor(UStaticMeshComponent* Mesh, bool bMerged) const
{
	if (!Mesh)
	{
		return;
	}

	// The engine's basic shape material exposes a "Color" parameter.
	if (UMaterialInstanceDynamic* Material = Mesh->CreateDynamicMaterialInstance(0))
	{
		Material->SetVectorParameterValue(TEXT("Color"), bMerged ? MergedColor : UnmergedColor);
	}
}

UStaticMeshComponent* ASSTestDummy::FindPartMesh(FName PartName) const
{
	if (PartName == ArmTag)
	{
		return Arm;
	}
	if (PartName == LegTag)
	{
		return Leg;
	}
	if (PartName == CoreTag)
	{
		return Core;
	}
	return nullptr;
}
