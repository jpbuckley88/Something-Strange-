#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SSEntityCharacter.generated.h"

class USSBodyPartComponent;
class UStaticMeshComponent;

/**
 * Base class for ghosts and demons. Chases Ned and attacks at close range.
 *
 * The behaviour here is deliberately simple and code-driven so entities work
 * with no content assets. It is meant to be replaced by StateTree behaviours
 * per entity type.
 *
 * Ground entities path with the navigation mesh, so the level needs a
 * NavMeshBoundsVolume. Flying entities steer straight at their target.
 */
UCLASS(Abstract)
class SOMETHINGSTRANGE_API ASSEntityCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASSEntityCharacter();

	UFUNCTION(BlueprintPure, Category = "Entity")
	USSBodyPartComponent* GetBodyParts() const { return BodyParts; }

	/** True if the part this entity attacks with is still attached. */
	UFUNCTION(BlueprintPure, Category = "Entity")
	bool CanAttack() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	bool bFlying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement", meta = (ClampMin = "0.0"))
	float MoveSpeed = 300.f;

	/** Ned is ignored beyond this distance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement", meta = (ClampMin = "0.0"))
	float AggroRange = 2500.f;

	/** Stand still while Ned has the goggles on. Used by the shade. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Movement")
	bool bFreezeWhileGogglesOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attack", meta = (ClampMin = "0.0"))
	float AttackDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attack", meta = (ClampMin = "0.0"))
	float AttackRange = 150.f;

	/** Seconds between attacks. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attack", meta = (ClampMin = "0.1"))
	float AttackInterval = 1.5f;

	/** Body part needed to attack. Destroy it and the entity is harmless. None means it can always attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity|Attack")
	FName AttackPartName;

	/** Residue left behind when banished. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity", meta = (ClampMin = "0"))
	int32 ResidueDrop = 1;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/**
	 * Adds an engine shape as a stand-in body part. Call from a subclass
	 * constructor. The mesh blocks only the laser, not movement.
	 */
	UStaticMeshComponent* AddPlaceholderPart(FName PartName, UStaticMesh* Shape, const FVector& Location, const FVector& Scale,
		float MaxHealth, bool bMerged, bool bIsCore);

	UPROPERTY(VisibleAnywhere, Category = "Entity")
	TObjectPtr<USSBodyPartComponent> BodyParts;

private:
	UFUNCTION()
	void HandleBanished();

	void MoveTowards(APawn* Target);
	void StopMoving();

	float TimeSinceAttack = 0.f;
	float TimeSinceRepath = 0.f;
};
