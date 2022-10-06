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
		: ItemIcon(nullptr)
		, Price(0)
		, Weight(0)
		, MaxStackAmount(0)
		, Amount(0)
		, bUniqueItem(0)
		, bStackable(0)
	{}

	FORCEINLINE bool IsValid() const { return ItemID != NAME_None && ItemIcon; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStackAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bStackable == false"))
	uint8 bUniqueItem:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUniqueItem == false"))
	uint8 bStackable:1;

	FString ToString() const
	{
		FString OutString;
		OutString += "{\n	ItemID: " + ItemID.ToString() + "; \n";
		OutString += "	Stack: " + FString::FromInt(Amount) + "; \n";
		OutString += "	MaxStackAmount: " + FString::FromInt(MaxStackAmount) + ";\n}";

		return OutString;
	}
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
		ItemDiscreption = InData.Description;
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

USTRUCT(BlueprintType)
struct FInventoryJson
{
	GENERATED_BODY()

public:

	FORCEINLINE bool IsValid() const { return !ItemID.IsEmpty() && SlotID > -1; }

	UPROPERTY(BlueprintReadOnly)
	FString ItemID;

	UPROPERTY(BlueprintReadOnly)
	int32 Stack;

	UPROPERTY(BlueprintReadOnly)
	int32 SlotID;
};