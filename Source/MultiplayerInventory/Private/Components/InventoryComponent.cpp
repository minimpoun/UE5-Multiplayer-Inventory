#include "Components/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::InitInventory_Implementation(const int32& InventorySize)
{
	Inventory.Empty();
	Inventory.Reserve(InventorySize);
	for (int32 i = 0; i < InventorySize; ++i)
	{
		Inventory.Add(FInventoryItemData());
	}
}

bool UInventoryComponent::InitInventory_Validate(const int32& InventorySize)
{
	return true;
}

void UInventoryComponent::SetItem(const int32& SlotID, const int32& Amount, const FName& Item)
{
	if (Inventory.IsValidIndex(SlotID))
	{
		if (const FInventoryItemData* ItemData = ItemTable->FindRow<FInventoryItemData>(Item, TEXT("")))
		{
			Inventory[SlotID] = *ItemData;
			Inventory[SlotID].Amount = Amount;
		}
	}
}

FInventoryItemData* UInventoryComponent::GetItem(const int32& SlotID)
{
	return &Inventory[SlotID];
}

void UInventoryComponent::ClearItem(const int32& SlotID)
{
	if (Inventory.IsValidIndex(SlotID))
	{
		Inventory[SlotID] = FInventoryItemData();
	}
}

int32 UInventoryComponent::FindFirstEmptySlot() const
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (!Inventory[i].IsValid())
		{
			return i;
		}
	}

	return -1;
}

TArray<int32> UInventoryComponent::UpdateStackSizes(const int32& AddAmount, const FName& ItemID)
{
	int32 LocalAddAmount = AddAmount;
	TArray<int32> UpdatedSlots;

	for (int32 i = 0; i < Inventory.Num(); ++i)
	{
		if (Inventory[i].ItemID == ItemID)
		{
			if (CanItemStack(i))
			{
				const int32 CachedInventoryAmount = Inventory[i].Amount;
				const int32 LeftOver = FMath::Abs((CachedInventoryAmount + LocalAddAmount) - Inventory[i].MaxStackAmount);

				Inventory[i].Amount = FMath::Clamp(CachedInventoryAmount + LocalAddAmount, 0, Inventory[i].MaxStackAmount);
				UpdatedSlots.Add(i);

				if (LeftOver == 0)
				{
					break;
				}

				LocalAddAmount = LeftOver;
			}
		}
	}

	return UpdatedSlots;
}

void UInventoryComponent::SetDataTable(const UDataTable* NewTable)
{
	ItemTable = NewTable;
}

bool UInventoryComponent::CanItemStack(const int32& SlotID) const
{
	return Inventory[SlotID].bStackable && (Inventory[SlotID].Amount < Inventory[SlotID].MaxStackAmount) && !Inventory[SlotID].bUniqueItem;
}

int32 UInventoryComponent::GetNumberFreeSlots() const
{
	int32 Out = 0;

	for (auto&& Element : Inventory)
	{
		if (!Element.IsValid())
		{
			++Out;
		}
	}

	return Out;
}

TArray<int32> UInventoryComponent::FindAllItemsByID(const FName& ItemID) const
{
	TArray<int32> OutSlots;
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].ItemID == ItemID)
		{
			OutSlots.Add(i);
		}
	}

	return OutSlots;
}

bool UInventoryComponent::IsSlotValid(const int32& SlotID) const
{
	return Inventory.IsValidIndex(SlotID) && Inventory[SlotID].IsValid();

}

int32 UInventoryComponent::GetSlotStack(const int32& SlotID)
{
	return IsSlotValid(SlotID) ? Inventory[SlotID].Amount : -1;
}

int32 UInventoryComponent::UpdateStackForSlot(const int32& Amount, const int32& SlotID)
{
	if (IsSlotValid(SlotID))
	{
		return Inventory[SlotID].Amount = FMath::Abs(Inventory[SlotID].Amount + Amount);
	}

	return -1;
}

bool UInventoryComponent::SwapItemData(const int32& FromItem, const int32& ToItem)
{
	if (IsSlotValid(FromItem) && IsSlotValid(ToItem))
	{
		Inventory.Swap(FromItem, ToItem);
		return true;
	}

	return false;
}

int32 UInventoryComponent::GetNumberOfItem(const FName& ItemID) const
{
	int32 Num = 0;

	if (ItemID.IsValid())
	{
		for (auto& Element : Inventory)
		{
			if (Element.ItemID == ItemID)
			{
				Num += Element.Amount;
			}
		}
	}

	return Num;
}

bool UInventoryComponent::ItemHasStackSpace(const FName& ItemID) const
{
	bool bFoundSpace = false;

	for (auto& Element : Inventory)
	{
		if (Element.ItemID == ItemID)
		{
			if (Element.Amount < Element.MaxStackAmount)
			{
				bFoundSpace = true;
				break;
			}
		}
	}

	return bFoundSpace;
}

bool UInventoryComponent::SlotsHaveFreeSpace(const TArray<int32>& Slots) const
{
	bool bFoundSpace = false;

	for (auto& Element : Slots)
	{
		if (Inventory[Element].Amount < Inventory[Element].MaxStackAmount)
		{
			bFoundSpace = true;
			break;
		}
	}

	return bFoundSpace;
}

void UInventoryComponent::ToString() const
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		GLog->Log(ELogVerbosity::Display, "Clientside inventory size: " + FString::FromInt(Inventory.Num()));
		for (int32 i = 0; i < Inventory.Num(); ++i)
		{
			GLog->Log(ELogVerbosity::Display, "Client: " + Inventory[i].ToString());
		}
	}
	else
	{
		GLog->Log(ELogVerbosity::Display, "Serverside inventory size: " + FString::FromInt(Inventory.Num()));
		for (int32 i = 0; i < Inventory.Num(); ++i)
		{
			GLog->Log(ELogVerbosity::Display, "Server: " + Inventory[i].ToString());
		}
	}
}

