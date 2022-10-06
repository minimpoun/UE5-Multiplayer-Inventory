// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Types/InventoryTypes.h"

#include "GameHUD.generated.h"

UCLASS()
class MULTIPLAYERINVENTORY_API AGameHUD final : public AHUD
{
	GENERATED_BODY()

public:
	void ShowForeignInventoryScreen();
	void HideForeignInventoryScreen();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryPanel();

	UFUNCTION(BlueprintCallable)
	void UpdateInventoryData(const int32& Data);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdatedInventoryData(const int32& Data);
	
};
