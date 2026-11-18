#include "Player/SSHealthComponent.h"

void USSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

void USSHealthComponent::ApplyDamage(float Amount)
{
	if (bDead || Amount <= 0.f)
	{
		return;
	}

	const float OldHealth = Health;
	Health = FMath::Max(0.f, Health - Amount);
	OnHealthChanged.Broadcast(Health, Health - OldHealth);

	if (Health <= 0.f)
	{
		bDead = true;
		OnDied.Broadcast();
	}
}

void USSHealthComponent::Heal(float Amount)
{
	if (bDead || Amount <= 0.f)
	{
		return;
	}

	const float OldHealth = Health;
	Health = FMath::Min(MaxHealth, Health + Amount);
	OnHealthChanged.Broadcast(Health, Health - OldHealth);
}
