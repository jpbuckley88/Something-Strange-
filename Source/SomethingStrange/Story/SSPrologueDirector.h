#pragma once

#include "CoreMinimal.h"
#include "Core/SSTypes.h"
#include "GameFramework/Actor.h"
#include "SSPrologueDirector.generated.h"

class ASSDoor;
class ASSPlayerCharacter;
class ASSPowerSwitch;
class ASSTutorialTarget;
class ASSWorkbench;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSSOnPrologueFinished);

/**
 * Runs the prologue from docs/Script.md: the phone call at the front door,
 * the shoulder, building and testing the goggles and the laser, and starting
 * the aperture. It sets objectives, shows control hints, plays the subtitled
 * lines, and moves on when the player does each thing.
 *
 * Place one in the level and point its properties at the actors for each
 * step. A step whose actor is not set is skipped, so the sequence can be
 * tested while the level is still being built.
 *
 * When the last switch is thrown it fires OnPrologueFinished. Bind that in
 * the level Blueprint to play the cutscene and start Level 1.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSPrologueDirector : public AActor
{
	GENERATED_BODY()

public:
	ASSPrologueDirector();

	/** Scene 1. Opening it ends the phone call. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|1 Front Door")
	TObjectPtr<ASSDoor> FrontDoor;

	/**
	 * Scene 2. Item the freezer pickup gives. Place an SS Pickup of type Part
	 * with this Item Id. Set to None to skip the step if there is no pickup yet.
	 */
	UPROPERTY(EditAnywhere, Category = "Prologue|2 Home")
	FName ShoulderItemId = TEXT("PorkShoulder");

	/** Scene 3. Any actor that generates overlaps, such as a Trigger Box at the bottom of the basement stairs. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|3 Lab")
	TObjectPtr<AActor> LabTrigger;

	/** Scenes 3 and 4. Its Required Items are the three parts to find. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|3 Lab")
	TObjectPtr<ASSWorkbench> GogglesWorkbench;

	/** Scene 5. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|5 Emitter")
	TObjectPtr<ASSWorkbench> LaserWorkbench;

	/** Scene 5. A merged, destructible target. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|5 Emitter")
	TObjectPtr<ASSTutorialTarget> PaintCan;

	/** Scene 6. A merged, indestructible target with Start Active off. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|6 Calibration")
	TObjectPtr<ASSTutorialTarget> Meat;

	/** Scenes 4 and 6. An unmerged, indestructible target: the mote in the jar. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|6 Calibration")
	TObjectPtr<ASSTutorialTarget> Specimen;

	/** Scene 7. All of them must be switched on. */
	UPROPERTY(EditInstanceOnly, Category = "Prologue|7 Aperture")
	TArray<TObjectPtr<ASSPowerSwitch>> ApertureSwitches;

	/** Fired when the aperture starts. The cutscene of Mom being taken goes here. */
	UPROPERTY(BlueprintAssignable, Category = "Prologue")
	FSSOnPrologueFinished OnPrologueFinished;

protected:
	virtual void BeginPlay() override;

private:
	enum class EStep : uint8
	{
		NotStarted,
		PhoneCall,
		GetShoulder,
		GoToLab,
		FindParts,
		AssembleGoggles,
		UseGoggles,
		GogglesRunOut,
		AssembleLaser,
		FirePaintCan,
		MaterialOnMeat,
		SpectralOnMeat,
		SpectralOnSpecimen,
		StartAperture,
		Finished
	};

	void Begin();
	void EnterStep(EStep NewStep);
	void Advance();
	bool IsStepAvailable(EStep InStep) const;
	void UpdatePartsObjective();

	UFUNCTION()
	void HandleDoorToggled(bool bOpen);

	UFUNCTION()
	void HandleInventoryChanged();

	UFUNCTION()
	void HandleLabTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void HandleAssembled(ASSWorkbench* Workbench);

	UFUNCTION()
	void HandleGogglesToggled(bool bActive);

	UFUNCTION()
	void HandleOverheatChanged(bool bOverheated);

	UFUNCTION()
	void HandlePaintCanDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void HandleFrequencyTested(ASSTutorialTarget* Target, ESSFrequency Frequency);

	UFUNCTION()
	void HandleSwitchChanged(bool bPowerOn);

	UPROPERTY(Transient)
	TObjectPtr<ASSPlayerCharacter> Ned;

	EStep Step = EStep::NotStarted;
	bool bShownOverheatHint = false;
};
