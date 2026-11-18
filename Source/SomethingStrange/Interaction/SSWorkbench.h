#pragma once

#include "CoreMinimal.h"
#include "Core/SSTypes.h"
#include "GameFramework/Actor.h"
#include "Interaction/SSInteractable.h"
#include "SSWorkbench.generated.h"

class ASSWorkbench;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnAssembled, ASSWorkbench*, Workbench);

/**
 * Where Ned builds something once he is carrying the parts for it. Used in
 * the prologue for the goggles and the laser.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSWorkbench : public AActor, public ISSInteractable
{
	GENERATED_BODY()

public:
	ASSWorkbench();

	virtual void Interact_Implementation(APawn* InteractingPawn) override;
	virtual FText GetInteractionPrompt_Implementation() const override;

	UFUNCTION(BlueprintPure, Category = "Workbench")
	bool IsAssembled() const { return bAssembled; }

	/** Items Ned must be carrying. Leave empty to need nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench")
	TArray<FName> RequiredItems;

	/** What is built: Goggles or Laser. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench")
	ESSPickupType Result = ESSPickupType::Goggles;

	/** Name used in the prompt, for example "goggles". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workbench")
	FText ResultName;

	UPROPERTY(BlueprintAssignable, Category = "Workbench")
	FSSOnAssembled OnAssembled;

private:
	UPROPERTY(VisibleAnywhere, Category = "Workbench")
	TObjectPtr<UStaticMeshComponent> Mesh;

	bool bAssembled = false;
};
