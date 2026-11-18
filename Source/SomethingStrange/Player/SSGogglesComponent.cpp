#include "Player/SSGogglesComponent.h"

#include "Engine/World.h"
#include "Spectral/SSSpectralSubsystem.h"

USSGogglesComponent::USSGogglesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USSGogglesComponent::BeginPlay()
{
	Super::BeginPlay();

	Charge = MaxCharge;
	TimeSinceDeactivated = RechargeDelay;
}

void USSGogglesComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bGogglesActive)
	{
		bGogglesActive = false;
		NotifySubsystem();
	}

	Super::EndPlay(EndPlayReason);
}

void USSGogglesComponent::ToggleGoggles()
{
	SetGogglesActive(!bGogglesActive);
}

bool USSGogglesComponent::SetGogglesActive(bool bNewActive)
{
	if (bNewActive == bGogglesActive)
	{
		return true;
	}

	if (bNewActive && Charge < MinChargeToActivate)
	{
		return false;
	}

	bGogglesActive = bNewActive;
	if (!bGogglesActive)
	{
		TimeSinceDeactivated = 0.f;
	}

	NotifySubsystem();
	OnGogglesToggled.Broadcast(bGogglesActive);
	return true;
}

void USSGogglesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bGogglesActive)
	{
		Charge = FMath::Max(0.f, Charge - DeltaTime);
		if (Charge <= 0.f)
		{
			SetGogglesActive(false);
		}
		return;
	}

	TimeSinceDeactivated += DeltaTime;
	if (TimeSinceDeactivated >= RechargeDelay)
	{
		Charge = FMath::Min(MaxCharge, Charge + RechargeRate * DeltaTime);
	}
}

void USSGogglesComponent::NotifySubsystem() const
{
	if (const UWorld* World = GetWorld())
	{
		if (USSSpectralSubsystem* Spectral = World->GetSubsystem<USSSpectralSubsystem>())
		{
			Spectral->SetSpectralVisionActive(bGogglesActive);
		}
	}
}
