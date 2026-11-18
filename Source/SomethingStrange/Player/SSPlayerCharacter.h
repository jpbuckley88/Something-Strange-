#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SSPlayerCharacter.generated.h"

class ASSLaserWeapon;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USSGogglesComponent;
class USSInteractionComponent;
struct FInputActionValue;

/**
 * Ned. First-person character carrying the goggles and the laser.
 *
 * Input actions and their default key bindings are built in code so the
 * project is playable with no content assets:
 *   WASD move, mouse look, Space jump, E interact,
 *   left mouse fire, right mouse goggles, Q switch frequency.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASSPlayerCharacter();

	UFUNCTION(BlueprintPure, Category = "Ned")
	USSGogglesComponent* GetGoggles() const { return Goggles; }

	UFUNCTION(BlueprintPure, Category = "Ned")
	USSInteractionComponent* GetInteraction() const { return Interaction; }

	UFUNCTION(BlueprintPure, Category = "Ned")
	ASSLaserWeapon* GetLaser() const { return Laser; }

	/** Laser class to spawn. Point this at a Blueprint subclass once the weapon has a mesh. */
	UPROPERTY(EditDefaultsOnly, Category = "Ned")
	TSubclassOf<ASSLaserWeapon> LaserClass;

	/** Where the laser sits relative to the camera. */
	UPROPERTY(EditDefaultsOnly, Category = "Ned")
	FVector LaserOffset = FVector(40.f, 20.f, -20.f);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void BuildDefaultInput();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();
	void StartFire();
	void StopFire();
	void ToggleGoggles();
	void CycleFrequency();

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<USSGogglesComponent> Goggles;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<USSInteractionComponent> Interaction;

	UPROPERTY(Transient)
	TObjectPtr<ASSLaserWeapon> Laser;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> GogglesAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> FrequencyAction;
};
