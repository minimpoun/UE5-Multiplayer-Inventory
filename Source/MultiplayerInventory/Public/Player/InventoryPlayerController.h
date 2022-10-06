#pragma once

#include "CoreMinimal.h"

#include "Components/InventoryEquipmentComponent.h"

#include "GameFramework/PlayerController.h"

#include "InventoryPlayerController.generated.h"

class AGameHUD;
class UInventoryComponent;
class UInventoryManagerComponent;

UCLASS()
class MULTIPLAYERINVENTORY_API AInventoryPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	AInventoryPlayerController();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryManagerComponent* GetInventoryManager() const noexcept;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryComponent* GetInventory() const noexcept;
	
	UFUNCTION(BlueprintCallable)
	void EnableUIControl();

	UFUNCTION(BlueprintCallable)
	void DisableUIControl();

	//~ Begin APlayerController Interface
	virtual void BeginPlay() override;
	//~ End APlayerController Interface
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UInventoryManagerComponent* InventoryManager;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	UInventoryEquipmentComponent* PlayerInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UDataTable> ItemDataTable;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<AGameHUD> GameHUD;
};