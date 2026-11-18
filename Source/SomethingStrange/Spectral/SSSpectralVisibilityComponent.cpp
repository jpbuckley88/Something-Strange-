#include "Spectral/SSSpectralVisibilityComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Spectral/SSSpectralSubsystem.h"

void USSSpectralVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	bool bActive = false;
	if (USSSpectralSubsystem* Spectral = GetWorld()->GetSubsystem<USSSpectralSubsystem>())
	{
		Spectral->OnSpectralVisionChanged.AddDynamic(this, &USSSpectralVisibilityComponent::HandleSpectralVisionChanged);
		bActive = Spectral->IsSpectralVisionActive();
	}

	ApplyVisibility(bActive);
}

void USSSpectralVisibilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (USSSpectralSubsystem* Spectral = World->GetSubsystem<USSSpectralSubsystem>())
		{
			Spectral->OnSpectralVisionChanged.RemoveDynamic(this, &USSSpectralVisibilityComponent::HandleSpectralVisionChanged);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void USSSpectralVisibilityComponent::HandleSpectralVisionChanged(bool bActive)
{
	ApplyVisibility(bActive);
}

void USSSpectralVisibilityComponent::ApplyVisibility(bool bVisible)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TInlineComponentArray<UPrimitiveComponent*> Primitives(Owner);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		Primitive->SetVisibility(bVisible);

		if (StencilValue > 0)
		{
			Primitive->SetRenderCustomDepth(bVisible);
			Primitive->SetCustomDepthStencilValue(StencilValue);
		}
	}
}
