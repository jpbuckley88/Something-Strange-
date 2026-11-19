#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/SSTypes.h"
#include "SSBodyPartComponent.generated.h"

class UMaterialInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSSOnPartMergeChanged, FName, PartName, bool, bMerged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnPartDestroyed, FName, PartName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSSOnPartDamaged, FName, PartName, ESSFrequency, Frequency, bool, bMatched, float, BaseDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSSOnBanished);

/**
 * Gives an entity targetable body parts, each either merged into our world or
 * still on the spirit frequency. Resolves laser hits to a part and applies
 * increased damage when the beam's frequency matches that part.
 *
 * Unmerged parts identified by component tag are hidden unless the goggles are
 * on. Skeletal meshes do the same per part through their material.
 */
UCLASS(ClassGroup = (SomethingStrange), meta = (BlueprintSpawnableComponent))
class SOMETHINGSTRANGE_API USSBodyPartComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USSBodyPartComponent();

	/**
	 * Applies beam damage to whichever part the hit landed on.
	 * Returns the damage dealt, or 0 if the hit was not on a live part.
	 */
	UFUNCTION(BlueprintCallable, Category = "Body Parts")
	float ApplyBeamDamage(const FHitResult& Hit, ESSFrequency Frequency, float BaseDamage);

	/** Index into Parts for the part a hit landed on, or INDEX_NONE. */
	UFUNCTION(BlueprintPure, Category = "Body Parts")
	int32 FindPartIndexFromHit(const FHitResult& Hit) const;

	UFUNCTION(BlueprintPure, Category = "Body Parts")
	int32 FindPartIndexByName(FName PartName) const;

	UFUNCTION(BlueprintPure, Category = "Body Parts")
	bool IsBanished() const { return bBanished; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
	TArray<FSSBodyPartState> Parts;

	/** Damage multiplier when the beam frequency matches the part. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts", meta = (ClampMin = "0.0"))
	float MatchedDamageMultiplier = 2.f;

	/** Damage multiplier when it does not. The wrong frequency does nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts", meta = (ClampMin = "0.0"))
	float MismatchedDamageMultiplier = 0.f;

	/** Destroy the owning actor when its core is destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
	bool bDestroyOwnerOnBanish = true;

	/**
	 * For stand-ins built from engine shapes: colour tagged components orange when
	 * merged and blue when unmerged, matching the laser's beam colours.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
	bool bTintPlaceholderMeshes = false;

	/** Translucent glow given to unmerged stand-in parts so they read as not fully here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
	TObjectPtr<UMaterialInterface> EtherealMaterial;

	/** How solid unmerged stand-in parts look through the goggles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EtherealOpacity = 0.35f;

	/** Fired when a part crosses between unmerged and merged. */
	UPROPERTY(BlueprintAssignable, Category = "Body Parts")
	FSSOnPartMergeChanged OnPartMergeChanged;

	/** Fired every time the beam lands on a part, with the damage before multipliers. */
	UPROPERTY(BlueprintAssignable, Category = "Body Parts")
	FSSOnPartDamaged OnPartDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Body Parts")
	FSSOnPartDestroyed OnPartDestroyed;

	/** Fired when the core is destroyed. */
	UPROPERTY(BlueprintAssignable, Category = "Body Parts")
	FSSOnBanished OnBanished;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleSpectralVisionChanged(bool bActive);

	/** Shows or hides tagged components to match merge state, goggles, and destruction. */
	void RefreshPartVisibility();

	void DestroyPart(int32 PartIndex);

	bool bSpectralVisionActive = false;
	bool bBanished = false;
};
