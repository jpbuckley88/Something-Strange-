#include "Player/SSInventoryComponent.h"

void USSInventoryComponent::GiveGoggles()
{
	if (!bHasGoggles)
	{
		bHasGoggles = true;
		OnInventoryChanged.Broadcast();
	}
}

void USSInventoryComponent::GiveLaser()
{
	if (!bHasLaser)
	{
		bHasLaser = true;
		OnInventoryChanged.Broadcast();
	}
}

void USSInventoryComponent::AddResidue(int32 Amount)
{
	if (Amount > 0)
	{
		Residue += Amount;
		OnInventoryChanged.Broadcast();
	}
}

bool USSInventoryComponent::SpendResidue(int32 Amount)
{
	if (Amount < 0 || Amount > Residue)
	{
		return false;
	}

	Residue -= Amount;
	OnInventoryChanged.Broadcast();
	return true;
}
