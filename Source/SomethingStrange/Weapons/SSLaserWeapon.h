#pragma once

#include "CoreMinimal.h"
#include "Core/SSTypes.h"
#include "GameFramework/Actor.h"
#include "SSLaserWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnFrequencyChanged, ESSFrequency, NewFrequency);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnOverheatChanged, bool, bOverheated);

/**
 * Ned's laser. A continuous beam with a heat meter in place of ammunition and
 * a frequency selector in place of ammunition types.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSLaserWeapon : public AActor
{
	GENERATED_BODY()

public:
	ASSLaserWeapon();

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void StopFire();

	/** Steps to the next frequency, wrapping round. */
	UFUNCTION(BlueprintCallable, Category = "Laser")
	void CycleFrequency();

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void SetFrequency(ESSFrequency NewFrequency);

	UFUNCTION(BlueprintPure, Category = "Laser")
	ESSFrequency GetFrequency() const { return CurrentFrequency; }

	/** True while the beam is actually on (trigger held and not overheated). */
	UFUNCTION(BlueprintPure, Category = "Laser")
	bool IsBeamActive() const { return bWantsToFire && !bOverheated; }

	UFUNCTION(BlueprintPure, Category = "Laser")
	bool IsOverheated() const { return bOverheated; }

	UFUNCTION(BlueprintPure, Category = "Laser")
	float GetHeatFraction() const { return MaxHeat > 0.f ? Heat / MaxHeat : 0.f; }

	UFUNCTION(BlueprintPure, Category = "Laser")
	FLinearColor GetBeamColor() const;

	/** True while the beam is landing on something that has body parts. */
	bool IsBeamOnEntity() const { return bBeamOnEntity; }

	/** The part under the beam, or None when the beam is on a section that is not a part. */
	FName GetBeamPartName() const { return BeamPartName; }

	bool IsBeamMatched() const { return bBeamMatched; }
	bool IsBeamPartMerged() const { return bBeamPartMerged; }
	float GetBeamPartHealthFraction() const { return BeamPartHealthFraction; }

	/** Damage per second before the matched or mismatched multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser", meta = (ClampMin = "0.0"))
	float DamagePerSecond = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser", meta = (ClampMin = "0.0"))
	float Range = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Heat", meta = (ClampMin = "1.0"))
	float MaxHeat = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Heat", meta = (ClampMin = "0.0"))
	float HeatPerSecond = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Heat", meta = (ClampMin = "0.0"))
	float CoolingPerSecond = 30.f;

	/** Seconds after the beam stops before cooling starts. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Heat", meta = (ClampMin = "0.0"))
	float CoolingDelay = 0.5f;

	/** After overheating, heat must fall to this fraction of the maximum before firing again. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Heat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float OverheatRecoverFraction = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Look")
	FLinearColor MaterialBeamColor = FLinearColor(1.f, 0.35f, 0.05f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Look")
	FLinearColor SpectralBeamColor = FLinearColor(0.1f, 0.9f, 1.f);

	/** Draw the beam as a debug line until it has real effects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Look")
	bool bDrawDebugBeam = true;

	UPROPERTY(BlueprintAssignable, Category = "Laser")
	FSSOnFrequencyChanged OnFrequencyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Laser")
	FSSOnOverheatChanged OnOverheatChanged;

	virtual void Tick(float DeltaTime) override;

private:
	void FireBeam(float DeltaTime);

	ESSFrequency CurrentFrequency = ESSFrequency::Material;
	float Heat = 0.f;
	float TimeSinceBeamStopped = 0.f;
	bool bWantsToFire = false;
	bool bOverheated = false;

	// What the beam landed on this frame.
	bool bBeamOnEntity = false;
	FName BeamPartName;
	bool bBeamMatched = false;
	bool bBeamPartMerged = false;
	float BeamPartHealthFraction = 0.f;
};
