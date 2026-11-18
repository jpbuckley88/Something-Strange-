#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSSOnInventoryChanged);

/** What Ned is carrying: his two inventions and the residue he has collected. */
UCLASS(ClassGroup = (SomethingStrange), meta = (BlueprintSpawnableComponent))
class SOMETHINGSTRANGE_API USSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GiveGoggles();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GiveLaser();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddResidue(int32 Amount);

	/** Returns false, spending nothing, if there is not enough. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SpendResidue(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasGoggles() const { return bHasGoggles; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasLaser() const { return bHasLaser; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetResidue() const { return Residue; }

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FSSOnInventoryChanged OnInventoryChanged;

private:
	bool bHasGoggles = false;
	bool bHasLaser = false;
	int32 Residue = 0;
};
