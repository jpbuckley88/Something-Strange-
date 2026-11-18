#include "Entities/SSWisp.h"

#include "Components/CapsuleComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ASSWisp::ASSWisp()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	GetCapsuleComponent()->InitCapsuleSize(30.f, 30.f);

	bFlying = true;
	MoveSpeed = 380.f;
	AttackDamage = 6.f;
	AttackRange = 110.f;
	AttackInterval = 1.f;
	AttackPartName = TEXT("Maw");
	ResidueDrop = 1;

	AddPlaceholderPart(TEXT("Core"), SphereMesh.Object, FVector::ZeroVector, FVector(0.5f), 40.f, false, true);
	AddPlaceholderPart(TEXT("Maw"), CubeMesh.Object, FVector(30.f, 0.f, -5.f), FVector(0.25f, 0.3f, 0.15f), 20.f, true, false);
}
