#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SSRift.generated.h"

class ASSEntityCharacter;
class ASSRiftAnchor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSSOnRiftSealed);

/**
 * A tear into the spirit realm. Spawns entities until every one of its anchors
 * has been destroyed, then seals. Sealing a level's rift ends the level.
 *
 * In the level: place the rift, place its anchors, and add them to Anchors.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSRift : public AActor
{
	GENERATED_BODY()

public:
	ASSRift();

	UFUNCTION(BlueprintPure, Category = "Rift")
	bool IsSealed() const { return bSealed; }

	UFUNCTION(BlueprintPure, Category = "Rift")
	int32 GetRemainingAnchors() const { return RemainingAnchors; }

	/** Start or stop spawning, for example when Ned enters the room. */
	UFUNCTION(BlueprintCallable, Category = "Rift")
	void SetSpawningEnabled(bool bEnabled);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Rift")
	TArray<TObjectPtr<ASSRiftAnchor>> Anchors;

	/** Picked from at random for each spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rift|Spawning")
	TArray<TSubclassOf<ASSEntityCharacter>> EntityClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rift|Spawning")
	bool bSpawnOnBeginPlay = true;

	/** Seconds between spawns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rift|Spawning", meta = (ClampMin = "0.5"))
	float SpawnInterval = 8.f;

	/** The rift waits while this many of its entities are still alive. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rift|Spawning", meta = (ClampMin = "1"))
	int32 MaxAlive = 4;

	/** Entities appear within this distance of the rift. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rift|Spawning", meta = (ClampMin = "0.0"))
	float SpawnRadius = 200.f;

	UPROPERTY(BlueprintAssignable, Category = "Rift")
	FSSOnRiftSealed OnRiftSealed;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleAnchorDestroyed(AActor* DestroyedActor);

	void SpawnEntity();
	void Seal();

	UPROPERTY(Transient)
	TArray<TObjectPtr<ASSEntityCharacter>> SpawnedEntities;

	FTimerHandle SpawnTimer;
	int32 RemainingAnchors = 0;
	bool bSealed = false;
};
