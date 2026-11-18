#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSInteractionComponent.generated.h"

/** Finds the interactable the player is looking at and uses it on request. */
UCLASS(ClassGroup = (SomethingStrange), meta = (BlueprintSpawnableComponent))
class SOMETHINGSTRANGE_API USSInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USSInteractionComponent();

	/** Uses the focused actor. Returns false if nothing is in focus. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool TryInteract();

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedActor() const { return FocusedActor.Get(); }

	/** Prompt of the focused actor, or empty text. */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FText GetFocusedPrompt() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float InteractionRange = 250.f;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TWeakObjectPtr<AActor> FocusedActor;
};
