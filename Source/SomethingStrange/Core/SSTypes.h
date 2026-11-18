#pragma once

#include "CoreMinimal.h"
#include "SSTypes.generated.h"

/**
 * Laser frequencies. Switching frequency is the game's ammunition change:
 * a beam does increased damage only to body parts on the same frequency.
 */
UENUM(BlueprintType)
enum class ESSFrequency : uint8
{
	/** Matches body parts that have fully merged into our world. */
	Material,
	/** Matches body parts still on the spirit frequency. */
	Spectral
};

/** What a pickup gives Ned. */
UENUM(BlueprintType)
enum class ESSPickupType : uint8
{
	Goggles,
	Laser,
	Residue,
	Health,
	/** A named item, such as a component for the workbench. */
	Part
};

/** One targetable part of an entity (an arm, the jaw, the core). */
USTRUCT(BlueprintType)
struct FSSBodyPartState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part")
	FName PartName;

	/** Skeleton bones belonging to this part. A hit on any child of these bones also counts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part")
	TArray<FName> Bones;

	/** Alternative to bones: any primitive component on the owner carrying this tag is this part. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part")
	FName ComponentTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.f;

	/** 0 = entirely on the spirit frequency, 1 = fully merged into our world. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MergeAmount = 0.f;

	/** Merge gained per second. 0 keeps the part in its starting state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part", meta = (ClampMin = "0.0"))
	float MergeRate = 0.f;

	/** Takes hits and reports them, but never loses health. For tutorial targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part")
	bool bIndestructible = false;

	/** Destroying the core banishes the whole entity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part")
	bool bIsCore = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Body Part")
	float Health = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Body Part")
	bool bDestroyed = false;

	/** A part counts as merged once it is at least half way through. */
	bool IsMerged() const { return MergeAmount >= 0.5f; }

	ESSFrequency GetMatchingFrequency() const
	{
		return IsMerged() ? ESSFrequency::Material : ESSFrequency::Spectral;
	}
};
