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

void USSInventoryComponent::RemoveLaser()
{
	if (bHasLaser)
	{
		bHasLaser = false;
		OnInventoryChanged.Broadcast();
	}
}

void USSInventoryComponent::RemoveEquipment()
{
	if (bHasGoggles || bHasLaser)
	{
		bHasGoggles = false;
		bHasLaser = false;
		OnInventoryChanged.Broadcast();
	}
}

void USSInventoryComponent::AddItem(FName ItemId)
{
	if (!ItemId.IsNone() && !Items.Contains(ItemId))
	{
		Items.Add(ItemId);
		OnInventoryChanged.Broadcast();
	}
}

int32 USSInventoryComponent::CountItems(const TArray<FName>& ItemIds) const
{
	int32 Count = 0;
	for (const FName& ItemId : ItemIds)
	{
		if (Items.Contains(ItemId))
		{
			++Count;
		}
	}
	return Count;
}
