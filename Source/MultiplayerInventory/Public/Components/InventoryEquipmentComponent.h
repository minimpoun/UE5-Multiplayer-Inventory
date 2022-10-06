#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"

#include "InventoryEquipmentComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYERINVENTORY_API UInventoryEquipmentComponent : public UInventoryComponent
{
	GENERATED_BODY()

public:
	UInventoryEquipmentComponent();

	void SetPawn(APawn* InPawn);
	
private:

	TObjectPtr<APawn> OwnerPawn;
};
