#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SSPlayerCharacter.generated.h"

class ASSLaserWeapon;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USSGogglesComponent;
class USSHealthComponent;
class USSInventoryComponent;
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

	UFUNCTION(BlueprintPure, Category = "Ned")
	USSHealthComponent* GetHealthComponent() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Ned")
	USSInventoryComponent* GetInventory() const { return Inventory; }

	/**
	 * Start with the goggles and the laser. Turn off in levels where Ned has to
	 * find them, such as the start of the house.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ned")
	bool bStartWithEquipment = true;

	/** Seconds between dying and the level restarting. */
	UPROPERTY(EditDefaultsOnly, Category = "Ned", meta = (ClampMin = "0.1"))
	float RestartDelay = 2.5f;

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

	UFUNCTION()
	void HandleDied();

	void RestartLevel();

	FTimerHandle RestartTimer;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<USSGogglesComponent> Goggles;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<USSInteractionComponent> Interaction;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<USSHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, Category = "Ned")
	TObjectPtr<USSInventoryComponent> Inventory;

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
