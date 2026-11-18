#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SSSpectralSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSSOnSpectralVisionChanged, bool, bActive);

/**
 * Single source of truth for whether the player is currently seeing on the
 * spirit frequency. The goggles write to it; anything spectral listens to it.
 */
UCLASS()
class SOMETHINGSTRANGE_API USSSpectralSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Spectral")
	void SetSpectralVisionActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Spectral")
	bool IsSpectralVisionActive() const { return bSpectralVisionActive; }

	UPROPERTY(BlueprintAssignable, Category = "Spectral")
	FSSOnSpectralVisionChanged OnSpectralVisionChanged;

private:
	bool bSpectralVisionActive = false;
};
