#include "Story/SSDialogueSubsystem.h"

#include "Engine/World.h"
#include "TimerManager.h"

void USSDialogueSubsystem::PlayLines(const TArray<FSSDialogueLine>& Lines)
{
	Queue.Append(Lines);

	if (!bPlaying)
	{
		PlayNext();
	}
}

void USSDialogueSubsystem::StopAll()
{
	Queue.Reset();
	bPlaying = false;

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LineTimer);
	}
}

void USSDialogueSubsystem::PlayNext()
{
	if (Queue.IsEmpty())
	{
		if (bPlaying)
		{
			bPlaying = false;
			OnDialogueFinished.Broadcast();
		}
		return;
	}

	CurrentLine = Queue[0];
	Queue.RemoveAt(0);
	bPlaying = true;

	// Reading time: a base second, plus a little for every character.
	float Duration = CurrentLine.Duration;
	if (Duration <= 0.f)
	{
		Duration = FMath::Max(1.8f, 1.f + 0.055f * CurrentLine.Text.ToString().Len());
	}

	GetWorld()->GetTimerManager().SetTimer(LineTimer, this, &USSDialogueSubsystem::PlayNext, Duration, false);
}
