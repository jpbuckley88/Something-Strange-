#include "Interaction/SSInteractionComponent.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Interaction/SSInteractable.h"

USSInteractionComponent::USSInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USSInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FocusedActor = nullptr;

	const APawn* Pawn = Cast<APawn>(GetOwner());
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	if (!Controller)
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(SSInteractionTrace), false, GetOwner());
	FHitResult Hit;
	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * InteractionRange;

	if (GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<USSInteractable>())
		{
			FocusedActor = HitActor;
		}
	}
}

bool USSInteractionComponent::TryInteract()
{
	AActor* Target = FocusedActor.Get();
	if (!Target)
	{
		return false;
	}

	ISSInteractable::Execute_Interact(Target, Cast<APawn>(GetOwner()));
	return true;
}

FText USSInteractionComponent::GetFocusedPrompt() const
{
	if (AActor* Target = FocusedActor.Get())
	{
		return ISSInteractable::Execute_GetInteractionPrompt(Target);
	}

	return FText::GetEmpty();
}
