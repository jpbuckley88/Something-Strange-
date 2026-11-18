#pragma once

#include "CoreMinimal.h"
#include "Core/SSTypes.h"
#include "GameFramework/Actor.h"
#include "SSTutorialTarget.generated.h"

class ASSTutorialTarget;
class USSBodyPartComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSSOnFrequencyTested, ASSTutorialTarget*, Target, ESSFrequency, Frequency);

/**
 * Something to point the laser at in the prologue: the paint can, the pork
 * shoulder, the specimen jar.
 *
 * It reports when it has been held under a given frequency for long enough,
 * which is how the tutorial knows the player has tried each setting. Change
 * the mesh on the Mesh component to dress it.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSTutorialTarget : public AActor
{
	GENERATED_BODY()

public:
	ASSTutorialTarget();

	/** Shows or hides the target and switches its collision to match. */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetTargetActive(bool bActive);

	/** Merged targets are solid and weak to the material beam. Unmerged ones show only through the goggles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bMerged = true;

	/** If set, the target survives any amount of fire. If not, it is destroyed at zero health. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bIndestructible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (ClampMin = "1.0"))
	float Health = 60.f;

	/** Seconds under one frequency before it counts as tested. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial", meta = (ClampMin = "0.1"))
	float SecondsToTest = 1.f;

	/** Hidden until the tutorial reaches it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bStartActive = true;

	/** Fired once per frequency, when the target has been under it for long enough. */
	UPROPERTY(BlueprintAssignable, Category = "Tutorial")
	FSSOnFrequencyTested OnFrequencyTested;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandlePartDamaged(FName PartName, ESSFrequency Frequency, bool bMatched, float BaseDamage);

	UPROPERTY(VisibleAnywhere, Category = "Tutorial")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Tutorial")
	TObjectPtr<USSBodyPartComponent> BodyParts;

	/** Seconds of beam received so far, per frequency. */
	TMap<ESSFrequency, float> Exposure;
	TSet<ESSFrequency> Tested;
};
