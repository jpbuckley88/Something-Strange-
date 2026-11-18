#include "UI/SSHUD.h"

#include "Core/SSTypes.h"
#include "Engine/Canvas.h"
#include "Interaction/SSInteractionComponent.h"
#include "Player/SSGogglesComponent.h"
#include "Player/SSPlayerCharacter.h"
#include "Weapons/SSLaserWeapon.h"

namespace
{
	const float BarWidth = 220.f;
	const float BarHeight = 14.f;
	const float Margin = 30.f;
}

void ASSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const ASSPlayerCharacter* Ned = Cast<ASSPlayerCharacter>(GetOwningPawn());
	if (!Ned)
	{
		return;
	}

	const float CenterX = Canvas->SizeX * 0.5f;
	const float CenterY = Canvas->SizeY * 0.5f;
	const ASSLaserWeapon* Laser = Ned->GetLaser();

	// Crosshair, tinted with the current beam colour.
	const FLinearColor CrosshairColor = Laser ? Laser->GetBeamColor() : FLinearColor::White;
	DrawRect(CrosshairColor, CenterX - 2.f, CenterY - 2.f, 4.f, 4.f);

	float Y = Canvas->SizeY - Margin - BarHeight;

	if (Laser)
	{
		const bool bSpectral = Laser->GetFrequency() == ESSFrequency::Spectral;
		const FString HeatLabel = Laser->IsOverheated() ? TEXT("HEAT - OVERHEATED") : TEXT("HEAT");
		DrawBar(Margin, Y, Laser->GetHeatFraction(), Laser->IsOverheated() ? FLinearColor::Red : FLinearColor(1.f, 0.6f, 0.1f), HeatLabel);
		Y -= BarHeight + 26.f;

		const FString FrequencyText = FString::Printf(TEXT("FREQUENCY: %s  [Q]"), bSpectral ? TEXT("SPECTRAL") : TEXT("MATERIAL"));
		DrawText(FrequencyText, Laser->GetBeamColor(), Margin, Y, nullptr, 1.2f);
		Y -= 30.f;
	}

	if (const USSGogglesComponent* Goggles = Ned->GetGoggles())
	{
		const FString GogglesLabel = Goggles->AreGogglesActive() ? TEXT("GOGGLES - ON") : TEXT("GOGGLES");
		DrawBar(Margin, Y, Goggles->GetChargeFraction(), FLinearColor(0.1f, 0.9f, 1.f), GogglesLabel);
	}

	if (const USSInteractionComponent* Interaction = Ned->GetInteraction())
	{
		const FText Prompt = Interaction->GetFocusedPrompt();
		if (!Prompt.IsEmpty())
		{
			DrawText(FString::Printf(TEXT("[E] %s"), *Prompt.ToString()), FLinearColor::White, CenterX + 20.f, CenterY + 20.f, nullptr, 1.2f);
		}
	}
}

void ASSHUD::DrawBar(float X, float Y, float Fraction, const FLinearColor& FillColor, const FString& Label)
{
	DrawText(Label, FLinearColor::White, X, Y - 18.f);
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.5f), X, Y, BarWidth, BarHeight);
	DrawRect(FillColor, X, Y, BarWidth * FMath::Clamp(Fraction, 0.f, 1.f), BarHeight);
}
