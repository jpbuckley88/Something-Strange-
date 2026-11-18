#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSTestDummy.generated.h"

class USSBodyPartComponent;
class UStaticMeshComponent;

/**
 * Stand-in entity built from engine shapes, for testing the goggles and the
 * laser before any art exists. Drag one into a level.
 *
 * - Torso (grey): not a part; the beam does nothing to it.
 * - Arm (orange): merged. Weak to the material frequency.
 * - Leg (blue, turning orange): unmerged at first, merges after a few seconds.
 * - Core (blue): unmerged, seen only through the goggles. Weak to the spectral
 *   frequency; destroying it banishes the dummy.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSTestDummy : public AActor
{
	GENERATED_BODY()

public:
	ASSTestDummy();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UStaticMeshComponent> Torso;

	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UStaticMeshComponent> Arm;

	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UStaticMeshComponent> Leg;

	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UStaticMeshComponent> Core;

	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<USSBodyPartComponent> BodyParts;
};
