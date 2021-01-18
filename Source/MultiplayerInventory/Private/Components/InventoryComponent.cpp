#include "Components/InventoryComponent.h"

void UInventoryComponent::InitInventory_Implementation(const int32& InventorySize)
{

}

bool UInventoryComponent::InitInventory_Validate(const int32& InventorySize)
{

}

void UInventoryComponent::SetItem(const int32& SlotID, const int32& Amount, const FName& Item)
{
	if (Inventory.IsValidIndex(SlotID))
	{
		FInventoryItemData* ItemData = ItemTable->FindRow<FInventoryItemData>(Item, TEXT(""));
		if (ItemData)
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

}

void UInventoryComponent::SetDataTable(const UDataTable* NewTable)
{

}

bool UInventoryComponent::CanItemStack(const int32& SlotID) const
{

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

}

int32 UInventoryComponent::GetSlotStack(const int32& SlotID)
{

}

int32 UInventoryComponent::UpdateStackForSlot(const int32& Amount, const int32& SlotID)
{

}

bool UInventoryComponent::SwapItemData(const int32& FromItem, const int32& ToItem)
{

}

int32 UInventoryComponent::GetNumberOfItem(const FName& ItemID) const
{

}

bool UInventoryComponent::ItemHasStackSpace(const FName& ItemID) const
{

}

bool UInventoryComponent::SlotsHaveFreeSpace(const TArray<int32>& Slots) const
{

}

void UInventoryComponent::ToString() const
{

}

