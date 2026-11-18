#include "World/SSRift.h"

#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Entities/SSEntityCharacter.h"
#include "SomethingStrange.h"
#include "TimerManager.h"
#include "World/SSRiftAnchor.h"

ASSRift::ASSRift()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASSRift::BeginPlay()
{
	Super::BeginPlay();

	for (ASSRiftAnchor* Anchor : Anchors)
	{
		if (Anchor)
		{
			++RemainingAnchors;
			Anchor->OnDestroyed.AddDynamic(this, &ASSRift::HandleAnchorDestroyed);
		}
	}

	if (RemainingAnchors == 0)
	{
		UE_LOG(LogSomethingStrange, Warning, TEXT("%s has no anchors assigned, so it can never be sealed."), *GetName());
	}

	if (bSpawnOnBeginPlay)
	{
		SetSpawningEnabled(true);
	}
}

void ASSRift::SetSpawningEnabled(bool bEnabled)
{
	FTimerManager& Timers = GetWorldTimerManager();

	if (bEnabled && !bSealed)
	{
		if (!Timers.IsTimerActive(SpawnTimer))
		{
			Timers.SetTimer(SpawnTimer, this, &ASSRift::SpawnEntity, SpawnInterval, true);
		}
	}
	else
	{
		Timers.ClearTimer(SpawnTimer);
	}
}

void ASSRift::SpawnEntity()
{
	if (EntityClasses.IsEmpty())
	{
		return;
	}

	// Forget entities that have been banished.
	SpawnedEntities.RemoveAll([](const ASSEntityCharacter* Entity)
	{
		return !IsValid(Entity);
	});

	if (SpawnedEntities.Num() >= MaxAlive)
	{
		return;
	}

	const TSubclassOf<ASSEntityCharacter> EntityClass = EntityClasses[FMath::RandHelper(EntityClasses.Num())];
	if (!EntityClass)
	{
		return;
	}

	const FVector2D Offset = FMath::RandPointInCircle(SpawnRadius);
	const FVector SpawnLocation = GetActorLocation() + FVector(Offset.X, Offset.Y, 0.f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ASSEntityCharacter* Entity = GetWorld()->SpawnActor<ASSEntityCharacter>(EntityClass, SpawnLocation, GetActorRotation(), SpawnParams))
	{
		SpawnedEntities.Add(Entity);
	}
}

void ASSRift::HandleAnchorDestroyed(AActor* DestroyedActor)
{
	if (bSealed)
	{
		return;
	}

	RemainingAnchors = FMath::Max(0, RemainingAnchors - 1);
	if (RemainingAnchors == 0)
	{
		Seal();
	}
}

void ASSRift::Seal()
{
	bSealed = true;
	SetSpawningEnabled(false);
	OnRiftSealed.Broadcast();
}
