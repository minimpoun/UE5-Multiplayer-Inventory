#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/InventoryTypes.h"

#include "InventoryManagerComponent.generated.h"

class UDataTable;
class UInventoryComponent;

USTRUCT()
struct FClientAddItemData
{
	GENERATED_BODY()

public:

	FClientAddItemData(){}
	FClientAddItemData(const int32& NewIndex, const int32& NewAmount, const FClientInventoryItem& NewClientData)
	{
		Index = NewIndex;
		Amount = NewAmount;
		ClientUIData = NewClientData;
	}

	UPROPERTY()
	int32 Index;

	UPROPERTY()
	int32 Amount;

	UPROPERTY()
	FClientInventoryItem ClientUIData;
};

UCLASS()
class UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UInventoryManagerComponent();

	virtual void SetInventoryComponent(UInventoryComponent* InPlayerInventory);
	virtual void SetItemDataTable(const UDataTable* NewDataTable);
	//virtual void SetupGameHUD()

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InitInventory();

	UFUNCTION(BlueprintCallable)
	void AddItem(const int32& Amount, const FName& Item);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(const int32& Amount, const int32& SlotID);

	UFUNCTION(BlueprintCallable)
	void SplitStack(const int32& SlotID, const int32& NewSlot);

	UFUNCTION(BlueprintCallable)
	void ClearInventoryItem(const int32& SlotID);

	UFUNCTION(BlueprintCallable)
	void ClearInventory();

	UFUNCTION(BlueprintCallable)
	void MoveItem(const int32& FromSlot, const int32& ToSlot);

	UFUNCTION(BlueprintCallable)
	TArray<int32> SearchForItemsByID(FName ID);

	UFUNCTION(BlueprintCallable)
	FInventoryItemData GetItemDataFromID(const FName& ItemID) const;

#if WITH_EDITOR
	UFUNCTION()
	void PrintInventory();
#endif // WITH_EDITOR

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 InventorySize = 32;

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddItem(const int32& Amount, const FName& Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RemoveItem(const int32& Amount, const int32& SlotID);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SplitStack(const int32& SlotID, const int32& NewSlot);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ClearInventoryItem(const int32& SlotID);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ClearInventory();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveItem(const int32& FromSlot, const int32& ToSlto);

	UFUNCTION(Client, Unreliable)
	void Client_UpdateSlot(const int32& SlotID, const int32& Amount, const FClientInventoryItem& NewUIData);

	UFUNCTION(Client, Unreliable)
	void Client_UpdateSlots(const TArray<FClientAddItemData>& NewItemRequests);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) override;

private:

	UPROPERTY()
	UInventoryComponent* PlayerInventory;

	UPROPERTY()
	UDataTable* ItemDataTable;

};