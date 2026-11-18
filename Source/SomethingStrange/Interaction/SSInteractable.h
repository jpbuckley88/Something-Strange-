#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SSInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class USSInteractable : public UInterface
{
	GENERATED_BODY()
};

/** Implement on anything Ned can use: doors, pickups, switches, the workbench. */
class SOMETHINGSTRANGE_API ISSInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APawn* InteractingPawn);

	/** Short text shown on the HUD while this is in focus, for example "Open door". */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPrompt() const;
};
