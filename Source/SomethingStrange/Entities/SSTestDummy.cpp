#include "Entities/SSTestDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Entities/SSBodyPartComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ASSTestDummy::ASSTestDummy()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	const FName ArmTag(TEXT("Arm"));
	const FName LegTag(TEXT("Leg"));
	const FName CoreTag(TEXT("Core"));

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
	BodyParts->bTintPlaceholderMeshes = true;

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
}
