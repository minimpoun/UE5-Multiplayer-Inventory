#include "Components/InventoryEquipmentComponent.h"

UInventoryEquipmentComponent::UInventoryEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryEquipmentComponent::SetPawn(APawn* InPawn)
{
	OwnerPawn = InPawn;
}

