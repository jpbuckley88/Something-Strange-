#include "Story/SSObjectiveSubsystem.h"

#include "Engine/World.h"

void USSObjectiveSubsystem::SetObjective(const FText& NewObjective)
{
	Objective = NewObjective;
	OnObjectiveChanged.Broadcast(Objective);
}

void USSObjectiveSubsystem::ClearObjective()
{
	SetObjective(FText::GetEmpty());
}

void USSObjectiveSubsystem::ShowHint(const FText& Hint, float Duration)
{
	CurrentHint = Hint;
	HintExpiresAt = GetWorld()->GetTimeSeconds() + Duration;
}

FText USSObjectiveSubsystem::GetHint() const
{
	const UWorld* World = GetWorld();
	if (World && World->GetTimeSeconds() < HintExpiresAt)
	{
		return CurrentHint;
	}

	return FText::GetEmpty();
}
