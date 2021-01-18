#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/InventoryTypes.h"

#include "InventoryComponent.generated.h"

class UDataTable;

UCLASS()
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(Server, Reliable, WithValidation)
	void InitInventory(const int32& InventorySize);

	void SetItem(const int32& SlotID, const int32& Amount, const FName& Item);
	FInventoryItemData* GetItem(const int32& SlotID);
	void ClearItem(const int32& SlotID);
	int32 FindFirstEmptySlot() const;
	TArray<int32> UpdateStackSizes(const int32& AddAmount, const FName& ItemID);
	void SetDataTable(const UDataTable* NewTable);
	bool CanItemStack(const int32& SlotID) const;
	int32 GetNumberFreeSlots() const;
	TArray<int32> FindAllItemsByID(const FName& ItemID) const;
	bool IsSlotValid(const int32& SlotID) const;
	int32 GetSlotStack(const int32& SlotID);
	int32 UpdateStackForSlot(const int32& Amount, const int32& SlotID);
	bool SwapItemData(const int32& FromItem, const int32& ToItem);
	int32 GetNumberOfItem(const FName& ItemID) const;
	bool ItemHasStackSpace(const FName& ItemID) const;
	bool SlotsHaveFreeSpace(const TArray<int32>& Slots) const;

	void ToString() const;

private:

	UPROPERTY()
	UDataTable* ItemTable;

	UPROPERTY()
	TArray<FInventoryItemData> Inventory;

};