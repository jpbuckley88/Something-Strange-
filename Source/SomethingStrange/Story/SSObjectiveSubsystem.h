#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SSObjectiveSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnObjectiveChanged, const FText&, NewObjective);

/**
 * What the HUD tells the player to do: one current objective, and short
 * control hints that fade after a few seconds.
 */
UCLASS()
class SOMETHINGSTRANGE_API USSObjectiveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void SetObjective(const FText& NewObjective);

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void ClearObjective();

	UFUNCTION(BlueprintPure, Category = "Objectives")
	FText GetObjective() const { return Objective; }

	/** Shows a control hint, such as "E to interact", for a few seconds. */
	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void ShowHint(const FText& Hint, float Duration = 7.f);

	/** The current hint, or empty text once it has expired. */
	UFUNCTION(BlueprintPure, Category = "Objectives")
	FText GetHint() const;

	UPROPERTY(BlueprintAssignable, Category = "Objectives")
	FSSOnObjectiveChanged OnObjectiveChanged;

private:
	FText Objective;
	FText CurrentHint;
	double HintExpiresAt = 0.0;
};
