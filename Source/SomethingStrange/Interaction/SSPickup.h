#pragma once

#include "CoreMinimal.h"
#include "Core/SSTypes.h"
#include "GameFramework/Actor.h"
#include "Interaction/SSInteractable.h"
#include "SSPickup.generated.h"

class ASSPlayerCharacter;
class USphereComponent;
class UStaticMeshComponent;

/**
 * Something Ned can pick up: the goggles, the laser, residue, or health.
 * Residue and health are collected by walking over them; equipment needs
 * the interact key.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSPickup : public AActor, public ISSInteractable
{
	GENERATED_BODY()

public:
	ASSPickup();

	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	ESSPickupType Type = ESSPickupType::Residue;

	/** Residue units or health points. Ignored for equipment. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "0"))
	int32 Amount = 1;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsCollectedOnTouch() const;
	void Collect(ASSPlayerCharacter* Ned);

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> TouchSphere;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
