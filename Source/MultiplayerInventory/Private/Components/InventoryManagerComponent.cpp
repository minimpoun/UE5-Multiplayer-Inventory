
#include "Components/InventoryManagerComponent.h"

#include "Components/InventoryComponent.h"
#include "LogCategories.h"

#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "InventoryManagerComponent"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInventoryManagerComponent::SetInventoryComponent(UInventoryComponent* InInventory)
{
	PlayerInventory = InInventory;
}

void UInventoryManagerComponent::SetItemDataTable(const UDataTable* NewDataTable)
{
	ItemDataTable = NewDataTable;
	PlayerInventory->SetDataTable(NewDataTable);
}

void UInventoryManagerComponent::AddInventoryItem(const int32& Amount, const FName& Item)
{
	if (Amount <= 0 || !Item.IsValid())
	{
		UE_LOG(LogInventoryError, Warning, TEXT("Could not add the item to the inventory because the amount was either less than zero or it was a invalid item"));
		return;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_AddInventoryItem(Amount, Item);
		return;
	}

	if (PlayerInventory->GetNumberFreeSlots() <= 0 /*|| !PlayerInventory->ItemHasStackSpace(Item)*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("not enough space in inventory"));
		return;
	}

	const FInventoryItemData   ItemData = GetItemDataFromID(Item);
	TArray<FClientAddItemData> ClientAddItemRequests;
	const FClientInventoryItem UIData = FClientInventoryItem(ItemData);

	if (ItemData.bStackable)
	{
		const TArray<int32> ItemSlots = PlayerInventory->FindAllItemsByID(Item);
		if (ItemSlots.Num() && PlayerInventory->SlotsHaveFreeSpace(ItemSlots))
		{
			TArray<int32> UpdatedSlots = PlayerInventory->UpdateStackSizes(Amount, Item);
			
			for (auto& Element : UpdatedSlots)
			{
				Client_UpdateSlot(Element, PlayerInventory->GetItem(Element)->Amount, UIData);
			}
		}
		else
		{
			int32 AddAmount = FMath::Clamp(Amount, 0, ItemData.MaxStackAmount);
			int32 LeftOver = FMath::Abs(Amount - AddAmount);
			int32 AddIndex = PlayerInventory->FindFirstEmptySlot();
			PlayerInventory->SetItem(AddIndex, AddAmount, Item);

			ClientAddItemRequests.Add(FClientAddItemData(AddIndex, AddAmount, UIData));
			
			while (LeftOver != 0)
			{
				AddAmount = FMath::Clamp(LeftOver, 0, ItemData.MaxStackAmount);
				AddIndex = PlayerInventory->FindFirstEmptySlot();
				PlayerInventory->SetItem(AddIndex, AddAmount, Item);
				ClientAddItemRequests.Add(FClientAddItemData(AddIndex, AddAmount, UIData));
				LeftOver = FMath::Abs(AddAmount - LeftOver);
			}

			if (ClientAddItemRequests.Num() == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("failed to add inventory item stacks"));
				return;
			}

			Client_UpdateSlots(ClientAddItemRequests);
		}

		return;
	}

	if (Amount > 1)
	{
		for (int32 i = 0; i < Amount; ++i)
		{
			int32 AddIndex = PlayerInventory->FindFirstEmptySlot();
			PlayerInventory->SetItem(AddIndex, 1, Item);
			ClientAddItemRequests.Add(FClientAddItemData(AddIndex, 1, UIData));
		}

		Client_UpdateSlots(ClientAddItemRequests);
	}
	else
	{
		const int32 AddIndex = PlayerInventory->FindFirstEmptySlot();
		PlayerInventory->SetItem(AddIndex, 1, Item);
		Client_UpdateSlot(AddIndex, 1, UIData);
	}
}

void UInventoryManagerComponent::RemoveInventoryItem(const int32& Amount, const int32& Slot)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_RemoveInventoryItem(Amount, Slot);
		return;
	}

	if (Slot > -1)
	{
		if (PlayerInventory->IsSlotValid(Slot))
		{
			if (const FInventoryItemData* OriginItem = PlayerInventory->GetItem(Slot); OriginItem->Amount < Amount)
			{
				if (PlayerInventory->GetNumberOfItem(OriginItem->ItemID) >= Amount)
				{
					TArray<int32> ItemIndexes = PlayerInventory->FindAllItemsByID(OriginItem->ItemID);

					ClearInventoryItem(Slot);

					int32 LeftOver = FMath::Abs(OriginItem->Amount - Amount);
					int32 CurrentIndex = 0;

					while (LeftOver != 0)
					{
						FInventoryItemData* TempItem = PlayerInventory->GetItem(ItemIndexes[CurrentIndex]);
						if (TempItem->IsValid())
						{
							const int32 CachedAmount = TempItem->Amount;
							TempItem->Amount = FMath::Clamp(CachedAmount - LeftOver, 0, TempItem->MaxStackAmount);
							LeftOver = FMath::Abs(CachedAmount - LeftOver);

							if (TempItem->Amount == 0)
							{
								ClearInventoryItem(CurrentIndex);
							}
							else
							{
								RemoveInventoryItem(LeftOver, CurrentIndex);
							}

							CurrentIndex++;
							continue;
						}

						CurrentIndex++;
					}

					return;
				}

				Client_DisplayToast(FText::Format(LOCTEXT("RemoveInventoryError", "You do not have {0} of {1}"), Amount, OriginItem->DisplayName));
				return;
			}

			const int32 AmountAfterUpdate = PlayerInventory->UpdateStackForSlot(-Amount, Slot);
			Client_UpdateSlot(Slot, AmountAfterUpdate, FClientInventoryItem(*PlayerInventory->GetItem(Slot)));
		}
	}
}

void UInventoryManagerComponent::SplitInventoryStack(const int32& Slot, const int32& NewSlot)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SplitInventoryStack(Slot, NewSlot);
		return;
	}

	if (PlayerInventory->IsSlotValid(Slot) && PlayerInventory->IsSlotValid(NewSlot))
	{
		if (const FInventoryItemData* OriginItem = PlayerInventory->GetItem(Slot); OriginItem->Amount > 1)
		{
			const int32 NewAmount = FMath::CeilToInt(OriginItem->Amount / 2.f);
			const int32 OldAmount = FMath::FloorToInt(OriginItem->Amount / 2.f);

			PlayerInventory->UpdateStackForSlot(-OldAmount, Slot);
			PlayerInventory->SetItem(NewSlot, NewAmount, OriginItem->ItemID);

			Client_UpdateSlot(Slot, OldAmount, FClientInventoryItem(*PlayerInventory->GetItem(Slot)));
			Client_UpdateSlot(NewSlot, NewAmount, FClientInventoryItem(*PlayerInventory->GetItem(NewSlot)));
		}
	}
}

void UInventoryManagerComponent::ClearInventoryItem(const int32& Slot)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_ClearInventoryItem(Slot);
		return;
	}

	if (PlayerInventory->IsSlotValid(Slot))
	{
		PlayerInventory->ClearItem(Slot);
		Client_ClearItem(Slot);
	}
}

void UInventoryManagerComponent::MoveInventoryItem(const int32& FromSlot, const int32& ToSlot)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_MoveInventoryItem(FromSlot, ToSlot);
		return;
	}

	if (PlayerInventory->IsSlotValid(FromSlot) && ToSlot > -1)
	{
		const FInventoryItemData* FromItemData = PlayerInventory->GetItem(FromSlot);
		if (PlayerInventory->IsSlotValid(ToSlot))
		{
			PlayerInventory->SwapItemData(FromSlot, ToSlot);
		}
		else
		{
			PlayerInventory->ClearItem(FromSlot);
			PlayerInventory->SetItem(ToSlot, FromItemData->Amount, FromItemData->ItemID);
		}

		Client_MoveInventoryItem(FromSlot, ToSlot);
	}
}

FInventoryItemData UInventoryManagerComponent::GetItemDataFromID(const FName& ItemID) const
{
	if (ItemID.IsValid() && ItemDataTable)
	{
		if (auto* ItemData = ItemDataTable->FindRow<FInventoryItemData>(ItemID, TEXT("")))
		{
			return *ItemData;
		}
	}

	return FInventoryItemData();
}

TArray<int32> UInventoryManagerComponent::SearchForItemsByID(FName ID)
{
	return {};
}

void UInventoryManagerComponent::InitInventoryFromDatabase_Implementation(const TArray<FInventoryJson>& InventoryJson)
{

}

bool UInventoryManagerComponent::InitInventoryFromDatabase_Validate(const TArray<FInventoryJson>& InventoryJson)
{
	return true;
}

void UInventoryManagerComponent::PrintInventory()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		PlayerInventory->ToString();
		Server_PrintInventory();
	}
	else
	{
		PlayerInventory->ToString();
	}
}

void UInventoryManagerComponent::Server_InitInventory_Implementation()
{
	PlayerInventory->InitInventory(InventorySize);
}

bool UInventoryManagerComponent::Server_InitInventory_Validate()
{
	return true;
}

void UInventoryManagerComponent::Server_AddInventoryItem_Implementation(const int32& Amount, const FName& Item)
{
	if (GetOwnerRole() == ROLE_Authority && Amount > 0 && Item.IsValid())
	{
		AddInventoryItem(Amount, Item);
	}
}

bool UInventoryManagerComponent::Server_AddInventoryItem_Validate(const int32& Amount, const FName& Item)
{
	return true;
}

void UInventoryManagerComponent::Server_RemoveInventoryItem_Implementation(const int32& Amount, const int32& Slot)
{
	RemoveInventoryItem(Amount, Slot);
}

bool UInventoryManagerComponent::Server_RemoveInventoryItem_Validate(const int32& Amount, const int32& Slot)
{
	return true;
}

void UInventoryManagerComponent::Server_SplitInventoryStack_Implementation(const int32& Slot, const int32& NewSlot)
{

}

bool UInventoryManagerComponent::Server_SplitInventoryStack_Validate(const int32& Slot, const int32& NewSlot)
{
	return true;
}

void UInventoryManagerComponent::Client_AddInventoryItem_Implementation(const int32& Slot, const int32& Amount, const FClientInventoryItem& Item)
{
	OnUpdateInventorySlot.Broadcast(Slot, Amount, Item);
}

void UInventoryManagerComponent::Client_RefreshSlots_Implementation(const TArray<int32>& SlotsToUpdate)
{
	OnRefreshSlots.Broadcast(SlotsToUpdate);
}

void UInventoryManagerComponent::Client_UpdateSlots_Implementation(const TArray<FClientAddItemData>& NewItemRequests)
{
	for (auto& Element : NewItemRequests)
	{
		OnUpdateInventorySlot.Broadcast(Element.Index, Element.AddAmount, Element.ClientUIData);
	}
}

void UInventoryManagerComponent::Server_PrintInventory_Implementation()
{
	PrintInventory();
}

bool UInventoryManagerComponent::Server_PrintInventory_Validate()
{
	return true;
}

void UInventoryManagerComponent::Client_UpdateSlot_Implementation(const int32& Slot, const int32& Amount, const FClientInventoryItem& UIData)
{
	OnUpdateInventorySlot.Broadcast(Slot, Amount, UIData);
}

void UInventoryManagerComponent::Client_ClearItem_Implementation(const int32& Slot)
{
	OnClearInventoryItems.Broadcast(Slot);
}

void UInventoryManagerComponent::Client_MoveInventoryItem_Implementation(const int32& FromSlot, const int32& ToSlot)
{
	OnMoveInventoryItems.Broadcast(FromSlot, ToSlot);
}

void UInventoryManagerComponent::Server_ClearInventoryItem_Implementation(const int32& Slot)
{
	ClearInventoryItem(Slot);
}

bool UInventoryManagerComponent::Server_ClearInventoryItem_Validate(const int32& Slot)
{
	return true;
}

void UInventoryManagerComponent::Client_DisplayToast_Implementation(const FText& Msg)
{
	if (!Msg.IsEmpty() && OnDisplayToast.IsBound())
	{
		OnDisplayToast.Broadcast(Msg);
	}
}

void UInventoryManagerComponent::Server_MoveInventoryItem_Implementation(const int32& FromSlot, const int32& ToSlot)
{
	MoveInventoryItem(FromSlot, ToSlot);
}

bool UInventoryManagerComponent::Server_MoveInventoryItem_Validate(const int32& FromSlot, const int32& ToSlot)
{
	return true;
}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryManagerComponent, InventorySize, COND_OwnerOnly);
}