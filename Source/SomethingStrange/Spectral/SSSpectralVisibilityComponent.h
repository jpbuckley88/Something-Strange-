#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SSSpectralVisibilityComponent.generated.h"

/**
 * Add to any actor that should only be seen through the goggles
 * (residue trails, sealed doors, rift anchors). Collision is unaffected,
 * so hidden things can still be shot and bumped into.
 */
UCLASS(ClassGroup = (SomethingStrange), meta = (BlueprintSpawnableComponent))
class SOMETHINGSTRANGE_API USSSpectralVisibilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Stencil value written while visible, for the goggles' post-process outline. 0 disables it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectral", meta = (ClampMin = "0", ClampMax = "255"))
	int32 StencilValue = 1;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleSpectralVisionChanged(bool bActive);

	void ApplyVisibility(bool bVisible);
};
