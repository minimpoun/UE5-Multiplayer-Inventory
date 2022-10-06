#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/InventoryTypes.h"

#include "InventoryManagerComponent.generated.h"

class UInventoryComponent;
class UDataTable;
class UInventoryEquipmentComponent;

USTRUCT()
struct FClientAddItemData
{
	GENERATED_BODY()

	FClientAddItemData(): Index(0), AddAmount(0)
	{
	}

	FClientAddItemData(const int32& NewIndex, const int32& NewAmount, const FClientInventoryItem& NewClientData)
	{
		Index = NewIndex;
		AddAmount = NewAmount;
		ClientUIData = NewClientData;
	}

	UPROPERTY()
	int32 Index;

	UPROPERTY()
	int32 AddAmount;

	UPROPERTY()
	FClientInventoryItem ClientUIData;
};

UCLASS()
class UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryManagerComponent();

	/** [client] */
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnUpdateInventorySlot, const int32&, const int32&, const FClientInventoryItem&);
	FOnUpdateInventorySlot OnUpdateInventorySlot;

	/** [client] */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMoveInventoryItems, const int32&, const int32&);
	FOnMoveInventoryItems OnMoveInventoryItems;

	/** [client] */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnClearInventoryItems, const int32&);
	FOnClearInventoryItems OnClearInventoryItems;

	/** [client] */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRefreshSlots, const TArray<int32>&);
	FOnRefreshSlots OnRefreshSlots;

	/** [client] */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDisplayToast, const FText&);
	FOnDisplayToast OnDisplayToast;

	virtual void SetInventoryComponent(UInventoryComponent* InInventory);
	virtual void SetItemDataTable(const UDataTable* NewDataTable);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InitInventory();

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 InventorySize = 30;

	/************************************************************************/
	/* Player Inventory                                                     */
	/************************************************************************/
	
	UFUNCTION(BlueprintCallable)
	void AddInventoryItem(const int32& Amount, const FName& Item);

	UFUNCTION(BlueprintCallable)
	void RemoveInventoryItem(const int32& Amount, const int32& Slot);

	void SplitInventoryStack(const int32& Slot, const int32& NewSlot);

	UFUNCTION(BlueprintCallable)
	void ClearInventoryItem(const int32& Slot);

	UFUNCTION(BlueprintCallable)
	void MoveInventoryItem(const int32& FromSlot, const int32& ToSlot);

	FInventoryItemData GetItemDataFromID(const FName& ItemID) const;

	UFUNCTION(BlueprintCallable)
	TArray<int32> SearchForItemsByID(FName ID);

	UFUNCTION(Server, WithValidation, Reliable)
	void InitInventoryFromDatabase(const TArray<FInventoryJson>& InventoryJson);

	UFUNCTION(BlueprintCallable)
	void PrintInventory();

protected:

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_PrintInventory();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddInventoryItem(const int32& Amount, const FName& Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveInventoryItem(const int32& Amount, const int32& Slot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SplitInventoryStack(const int32& Slot, const int32& NewSlot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ClearInventoryItem(const int32& Slot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveInventoryItem(const int32& FromSlot, const int32& ToSlot);

	UFUNCTION(Client, Reliable)
	void Client_DisplayToast(const FText& Msg);

	UFUNCTION(Client, Reliable)
	void Client_ClearItem(const int32& Slot);

	UFUNCTION(Client, Reliable)
	void Client_AddInventoryItem(const int32& Slot, const int32& Amount, const FClientInventoryItem& Item);

	UFUNCTION(Client, Reliable)
	void Client_RefreshSlots(const TArray<int32>& SlotsToUpdate);

	UFUNCTION(Client, Reliable)
	void Client_UpdateSlot(const int32& Slot, const int32& Amount, const FClientInventoryItem& UIData);

	UFUNCTION(Client, Reliable)
	void Client_UpdateSlots(const TArray<FClientAddItemData>& NewItemRequests);
	
	UFUNCTION(Client, Reliable)
	void Client_MoveInventoryItem(const int32& FromSlot, const int32& ToSlot);

	//~ Begin UActorComponent Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UActorComponent Interface
	
	UPROPERTY()
	const UDataTable* ItemDataTable;
	
private:

	TObjectPtr<UInventoryComponent> PlayerInventory;
};