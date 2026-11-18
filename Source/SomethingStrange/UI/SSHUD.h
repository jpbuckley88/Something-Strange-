#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SSHUD.generated.h"

/**
 * Canvas-drawn placeholder HUD: crosshair, goggle charge, laser heat and
 * frequency, interaction prompt. To be replaced by UMG widgets.
 */
UCLASS()
class SOMETHINGSTRANGE_API ASSHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawBar(float X, float Y, float Fraction, const FLinearColor& FillColor, const FString& Label);
};
