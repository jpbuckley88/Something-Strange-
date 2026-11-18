#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SSDialogueSubsystem.generated.h"

/** One subtitled line. */
USTRUCT(BlueprintType)
struct FSSDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText Text;

	/** Seconds on screen. 0 works it out from the length of the line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (ClampMin = "0.0"))
	float Duration = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSSOnDialogueFinished);

/**
 * Plays subtitled lines one after another. Text only for now; voice audio
 * hooks in here later.
 */
UCLASS()
class SOMETHINGSTRANGE_API USSDialogueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Adds lines to the end of the queue and starts playing if idle. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void PlayLines(const TArray<FSSDialogueLine>& Lines);

	/** Drops the current line and everything queued. */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StopAll();

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsPlaying() const { return bPlaying; }

	/** The line on screen now. Only meaningful while IsPlaying is true. */
	UFUNCTION(BlueprintPure, Category = "Dialogue")
	FSSDialogueLine GetCurrentLine() const { return CurrentLine; }

	/** Fired when the queue runs dry. */
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FSSOnDialogueFinished OnDialogueFinished;

private:
	void PlayNext();

	TArray<FSSDialogueLine> Queue;
	FSSDialogueLine CurrentLine;
	FTimerHandle LineTimer;
	bool bPlaying = false;
};
