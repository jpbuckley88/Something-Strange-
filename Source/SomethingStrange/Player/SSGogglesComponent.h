#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSGogglesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnGogglesToggled, bool, bActive);

/**
 * Ned's goggles: see on the spirit frequency for as long as the charge lasts.
 * Charge is measured in seconds of use.
 */
UCLASS(ClassGroup = (SomethingStrange), meta = (BlueprintSpawnableComponent))
class SOMETHINGSTRANGE_API USSGogglesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USSGogglesComponent();

	UFUNCTION(BlueprintCallable, Category = "Goggles")
	void ToggleGoggles();

	/** Returns false if switching on was refused because the charge is too low. */
	UFUNCTION(BlueprintCallable, Category = "Goggles")
	bool SetGogglesActive(bool bNewActive);

	UFUNCTION(BlueprintPure, Category = "Goggles")
	bool AreGogglesActive() const { return bGogglesActive; }

	UFUNCTION(BlueprintPure, Category = "Goggles")
	float GetCharge() const { return Charge; }

	UFUNCTION(BlueprintPure, Category = "Goggles")
	float GetChargeFraction() const { return MaxCharge > 0.f ? Charge / MaxCharge : 0.f; }

	/** Seconds of vision on a full charge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goggles", meta = (ClampMin = "0.1"))
	float MaxCharge = 5.f;

	/** Seconds of charge regained per second while switched off. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goggles", meta = (ClampMin = "0.0"))
	float RechargeRate = 0.75f;

	/** Seconds after switching off before recharging starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goggles", meta = (ClampMin = "0.0"))
	float RechargeDelay = 1.5f;

	/** Charge needed to switch on, so the goggles cannot be flickered at empty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goggles", meta = (ClampMin = "0.0"))
	float MinChargeToActivate = 1.f;

	UPROPERTY(BlueprintAssignable, Category = "Goggles")
	FSSOnGogglesToggled OnGogglesToggled;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void NotifySubsystem() const;

	float Charge = 0.f;
	float TimeSinceDeactivated = 0.f;
	bool bGogglesActive = false;
};
