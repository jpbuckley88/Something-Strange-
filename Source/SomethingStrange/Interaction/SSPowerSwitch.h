#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SSInteractable.h"
#include "SSPowerSwitch.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnPowerChanged, bool, bPowerOn);

/**
 * A switch such as the house's breaker. Bind OnPowerChanged in the level
 * Blueprint to turn lights on, unlock doors, and so on.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSPowerSwitch : public AActor, public ISSInteractable
{
	GENERATED_BODY()

public:
	ASSPowerSwitch();

	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Power")
	void SetPowerOn(bool bNewPowerOn);

	UFUNCTION(BlueprintPure, Category = "Power")
	bool IsPowerOn() const { return bPowerOn; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	bool bPowerOn = false;

	/** If set, the switch can be turned on but not off again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	bool bOneWay = true;

	UPROPERTY(BlueprintAssignable, Category = "Power")
	FSSOnPowerChanged OnPowerChanged;

private:
	UPROPERTY(VisibleAnywhere, Category = "Power")
	TObjectPtr<UStaticMeshComponent> SwitchMesh;
};
