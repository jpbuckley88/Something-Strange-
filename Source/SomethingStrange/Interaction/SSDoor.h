#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SSInteractable.h"
#include "SSDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnDoorToggled, bool, bOpen);

/**
 * A hinged door. The actor's origin is the hinge, at floor level. The default
 * mesh is a 100 x 10 x 200 slab; swap it in a Blueprint subclass.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSDoor : public AActor, public ISSInteractable
{
	GENERATED_BODY()

public:
	ASSDoor();

	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetOpen(bool bNewOpen);

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetLocked(bool bNewLocked) { bLocked = bNewLocked; }

	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsOpen() const { return bOpen; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bLocked = false;

	/** Yaw of the open door, in degrees. Negative swings the other way. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenAngle = 95.f;

	/** Degrees per second. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "1.0"))
	float SwingSpeed = 180.f;

	UPROPERTY(BlueprintAssignable, Category = "Door")
	FSSOnDoorToggled OnDoorToggled;

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> Hinge;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	bool bOpen = false;
	float CurrentAngle = 0.f;
};
