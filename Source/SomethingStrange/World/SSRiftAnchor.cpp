#include "World/SSRiftAnchor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Entities/SSBodyPartComponent.h"
#include "UObject/ConstructorHelpers.h"

ASSRiftAnchor::ASSRiftAnchor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	const FName AnchorTag(TEXT("Anchor"));

	// Blocks only the laser, so Ned cannot find anchors by walking into them.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetStaticMesh(SphereMesh.Object);
	Mesh->SetRelativeScale3D(FVector(0.6f));
	Mesh->ComponentTags.Add(AnchorTag);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	BodyParts = CreateDefaultSubobject<USSBodyPartComponent>(TEXT("BodyParts"));
	BodyParts->bTintPlaceholderMeshes = true;

	// A single unmerged core: destroying it destroys the anchor.
	FSSBodyPartState AnchorPart;
	AnchorPart.PartName = AnchorTag;
	AnchorPart.ComponentTag = AnchorTag;
	AnchorPart.MaxHealth = 120.f;
	AnchorPart.MergeAmount = 0.f;
	AnchorPart.bIsCore = true;
	BodyParts->Parts.Add(AnchorPart);
}
