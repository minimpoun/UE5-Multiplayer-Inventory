#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryComponent.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInventoryManagerComponent::Server_InitInventory_Implementation()
{

}

bool UInventoryManagerComponent::Server_InitInventory_Validate()
{
	return true;
}

void UInventoryManagerComponent::SetInventoryComponent(UInventoryComponent* InPlayerInventory)
{
	PlayerInventory = InPlayerInventory;
}

void UInventoryManagerComponent::SetItemDataTable(const UDataTable* NewDataTable)
{
	
}

void UInventoryManagerComponent::AddItem(const int32& Amount, const FName& Item)
{
	if (Amount <= 0 || !Item.IsValid()) return;
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_AddItem(Amount, Item);
		return;
	}

	if (PlayerInventory->GetNumberFreeSlots() <= 0 || !PlayerInventory->ItemHasStackSpace(Item)) return;

	FInventoryItemData ItemData = GetItemDataFromID(Item);
	FClientInventoryItem UIData = FClientInventoryItem(ItemData);
	TArray<FClientAddItemData> ClientAddItemRequests;

	if (ItemData.bStackable)
	{
		TArray<int32> ItemSlots = PlayerInventory->FindAllItemsByID(Item);
		if (ItemSlots.Num() && PlayerInventory->SlotsHaveFreeSpace(ItemSlots))
		{
			TArray<int32> UpdateSlots = PlayerInventory->UpdateStackSizes(Amount, Item);

			for (auto& Element : UpdateSlots)
			{
				Client_UpdateSlot(Element, PlayerInventory->GetItem(Element)->Amount, UIData);
			}

			return;
		}
		else
		{
			int32 AddAmount = FMath::Clamp(Amount, 0, ItemData.MaxStackSize);
			int32 LeftOver = FMath::Abs(Amount - AddAmount);
			int32 AddIndex = PlayerInventory->FindFirstEmptySlot();
			PlayerInventory->SetItem(AddIndex, AddAmount, Item);

			ClientAddItemRequests.Add(FClientAddItemData(AddIndex, AddAmount, UIData));

			while (LeftOver != 0)
			{
				AddAmount = FMath::Clamp(LeftOver, 0, ItemData.MaxStackSize);
				AddIndex = PlayerInventory->FindFirstEmptySlot();
				PlayerInventory->SetItem(AddIndex, AddAmount, Item);
				ClientAddItemRequests.Add(FClientAddItemData(AddIndex, AddAmount, UIData));
				LeftOver = FMath::Abs(AddAmount - LeftOver);
			}

			if (ClientAddItemRequests.Num() == 0)
			{
				// #@TODO(Chrisr): add logging
				return;
			}

			Client_UpdateSlots(ClientAddItemRequests);
		}

		return;
	}

	if (Amount > 1)
	{
		for (int32 i = 0; i < Amount; i++)
		{
			int32 AddIndex = PlayerInventory->FindFirstEmptySlot();
			PlayerInventory->SetItem(AddIndex, 1, Item);
			ClientAddItemRequests.Add(FClientAddItemData(AddIndex, 1, UIData));
		}

		Client_UpdateSlots(ClientAddItemRequests);
		return;
	}
	else
	{
		int32 AddIndex = PlayerInventory->FindFirstEmptySlot();
		PlayerInventory->SetItem(AddIndex, 1, Item);
		Client_UpdateSlot(AddIndex, 1, UIData);
	}
}

void UInventoryManagerComponent::RemoveItem(const int32& Amount, const int32& SlotID)
{

}

void UInventoryManagerComponent::SplitStack(const int32& SlotID, const int32& NewSlot)
{

}

void UInventoryManagerComponent::ClearInventoryItem(const int32& SlotID)
{

}

void UInventoryManagerComponent::ClearInventory()
{

}

void UInventoryManagerComponent::MoveItem(const int32& FromSlot, const int32& ToSlot)
{

}

TArray<int32> UInventoryManagerComponent::SearchForItemsByID(FName ID)
{

}

FInventoryItemData UInventoryManagerComponent::GetItemDataFromID(const FName& ItemID) const
{
	if (ItemID.IsValid() && ItemDataTable)
	{
		auto* ItemData = ItemDataTable->FindRow<FInventoryItemData>(ItemID, TEXT(""));
		if (ItemData)
		{
			return *ItemData;
		}
	}

	return FInventoryItemData();
}

void UInventoryManagerComponent::PrintInventory()
{

}

void UInventoryManagerComponent::Server_AddItem_Implementation(const int32& Amount, const FName& Item)
{

}

bool UInventoryManagerComponent::Server_AddItem_Validate(const int32& Amount, const FName& Item)
{

}

void UInventoryManagerComponent::Server_RemoveItem_Implementation(const int32& Amount, const int32& SlotID)
{

}

bool UInventoryManagerComponent::Server_RemoveItem_Validate(const int32& Amount, const int32& SlotID)
{

}

void UInventoryManagerComponent::Server_SplitStack_Implementation(const int32& SlotID, const int32& NewSlot)
{

}

bool UInventoryManagerComponent::Server_SplitStack_Validate(const int32& SlotID, const int32& NewSlot)
{

}

void UInventoryManagerComponent::Server_ClearInventoryItem_Implementation(const int32& SlotID)
{

}

bool UInventoryManagerComponent::Server_ClearInventoryItem_Validate(const int32& SlotID)
{

}

void UInventoryManagerComponent::Server_ClearInventory_Implementation()
{

}

bool UInventoryManagerComponent::Server_ClearInventory_Validate()
{

}

void UInventoryManagerComponent::Server_MoveItem_Implementation(const int32& FromSlot, const int32& ToSlto)
{

}

bool UInventoryManagerComponent::Server_MoveItem_Validate(const int32& FromSlot, const int32& ToSlto)
{

}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)
{

}
