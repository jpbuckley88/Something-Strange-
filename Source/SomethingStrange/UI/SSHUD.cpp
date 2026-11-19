#include "UI/SSHUD.h"

#include "Core/SSTypes.h"
#include "Engine/Canvas.h"
#include "Interaction/SSInteractionComponent.h"
#include "Player/SSGogglesComponent.h"
#include "Player/SSHealthComponent.h"
#include "Player/SSInventoryComponent.h"
#include "Player/SSPlayerCharacter.h"
#include "Story/SSDialogueSubsystem.h"
#include "Story/SSObjectiveSubsystem.h"
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
	const USSInventoryComponent* Inventory = Ned->GetInventory();
	const USSHealthComponent* Health = Ned->GetHealthComponent();
	const ASSLaserWeapon* Laser = Inventory->HasLaser() ? Ned->GetLaser() : nullptr;

	if (Health->IsDead())
	{
		DrawRect(FLinearColor(0.3f, 0.f, 0.f, 0.6f), 0.f, 0.f, Canvas->SizeX, Canvas->SizeY);
		DrawText(TEXT("NED IS DEAD"), FLinearColor::Red, CenterX - 110.f, CenterY - 20.f, nullptr, 2.5f);
		return;
	}

	DrawStory();

	// Crosshair, tinted with the current beam colour.
	const FLinearColor CrosshairColor = Laser ? Laser->GetBeamColor() : FLinearColor::White;
	DrawRect(CrosshairColor, CenterX - 2.f, CenterY - 2.f, 4.f, 4.f);

	// Stand-in for hit effects and sound: says what the beam is on and whether the frequency is right.
	if (Laser && Laser->IsBeamOnEntity())
	{
		const float ReadoutY = CenterY + 40.f;
		if (Laser->GetBeamPartName().IsNone())
		{
			DrawCenteredText(TEXT("NO EFFECT - not a weak point"), FLinearColor(0.7f, 0.7f, 0.7f), ReadoutY, 1.3f);
		}
		else
		{
			const FString PartText = FString::Printf(TEXT("%s (%s)  %d%%"),
				*Laser->GetBeamPartName().ToString().ToUpper(),
				Laser->IsBeamPartMerged() ? TEXT("merged") : TEXT("unmerged"),
				FMath::CeilToInt(Laser->GetBeamPartHealthFraction() * 100.f));
			DrawCenteredText(PartText, FLinearColor::White, ReadoutY, 1.3f);

			if (Laser->IsBeamMatched())
			{
				DrawCenteredText(TEXT("MATCHED - full damage"), FLinearColor(0.3f, 1.f, 0.3f), ReadoutY + 24.f, 1.3f);
			}
			else
			{
				DrawCenteredText(TEXT("WRONG FREQUENCY - no damage, press Q"), FLinearColor(1.f, 0.2f, 0.2f), ReadoutY + 24.f, 1.3f);
			}
		}
	}

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

	if (Inventory->HasGoggles())
	{
		const USSGogglesComponent* Goggles = Ned->GetGoggles();
		const FString GogglesLabel = Goggles->AreGogglesActive() ? TEXT("GOGGLES - ON") : TEXT("GOGGLES");
		DrawBar(Margin, Y, Goggles->GetChargeFraction(), FLinearColor(0.1f, 0.9f, 1.f), GogglesLabel);
		Y -= BarHeight + 26.f;
	}

	DrawBar(Margin, Y, Health->GetHealthFraction(), FLinearColor(0.8f, 0.05f, 0.05f), TEXT("HEALTH"));

	const FString ResidueText = FString::Printf(TEXT("RESIDUE: %d"), Inventory->GetResidue());
	DrawText(ResidueText, FLinearColor(0.4f, 1.f, 0.2f), Canvas->SizeX - Margin - 160.f, Canvas->SizeY - Margin - 20.f, nullptr, 1.2f);

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

void ASSHUD::DrawCenteredText(const FString& Text, const FLinearColor& Color, float Y, float Scale)
{
	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Text, Width, Height, nullptr, Scale);
	DrawText(Text, Color, (Canvas->SizeX - Width) * 0.5f, Y, nullptr, Scale);
}

void ASSHUD::DrawStory()
{
	const UWorld* World = GetWorld();

	if (const USSObjectiveSubsystem* Objectives = World->GetSubsystem<USSObjectiveSubsystem>())
	{
		const FText Objective = Objectives->GetObjective();
		if (!Objective.IsEmpty())
		{
			DrawText(TEXT("OBJECTIVE"), FLinearColor(1.f, 0.9f, 0.2f), Margin, Margin);
			DrawText(Objective.ToString(), FLinearColor::White, Margin, Margin + 18.f, nullptr, 1.3f);
		}

		const FText Hint = Objectives->GetHint();
		if (!Hint.IsEmpty())
		{
			DrawCenteredText(Hint.ToString(), FLinearColor(1.f, 0.9f, 0.2f), Canvas->SizeY * 0.68f, 1.4f);
		}
	}

	if (const USSDialogueSubsystem* Dialogue = World->GetSubsystem<USSDialogueSubsystem>())
	{
		if (Dialogue->IsPlaying())
		{
			const FSSDialogueLine Line = Dialogue->GetCurrentLine();
			const FString Subtitle = FString::Printf(TEXT("%s: %s"), *Line.Speaker.ToString(), *Line.Text.ToString());
			DrawCenteredText(Subtitle, FLinearColor::White, Canvas->SizeY * 0.8f, 1.4f);
		}
	}
}
