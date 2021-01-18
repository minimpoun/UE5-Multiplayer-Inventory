#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "InventoryTypes.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemData : public FTableRowBase
{
	GENERATED_BODY()

public:

	FInventoryItemData()
	{
		ItemID = NAME_None;
		ItemIcon = nullptr;
	}

	FORCEINLINE bool IsValid() const { return ItemID != NAME_None && ItemIcon; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Discription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bStackable == false"))
	uint8 bUniqueItem:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUniqueItem == false"))
	uint8 bStackable:1;
};

USTRUCT(BlueprintType)
struct FClientInventoryItem
{
	GENERATED_BODY()

public:

	FClientInventoryItem(){}
	FClientInventoryItem(const FInventoryItemData& InData)
	{
		ItemID = InData.ItemID;
		ItemIcon = InData.ItemIcon;
		ItemDiscreption = InData.Discription;
		ItemName = InData.DisplayName;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ItemDiscreption;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText ItemName;
};