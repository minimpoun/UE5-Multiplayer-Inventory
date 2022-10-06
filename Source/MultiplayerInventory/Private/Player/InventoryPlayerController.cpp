#include "Player/InventoryPlayerController.h"

#include "Components/InventoryEquipmentComponent.h"
#include "Components/InventoryManagerComponent.h"

#include "Player/MultiplayerInventoryCharacter.h"

#include "UserInterface/GameHUD.h"

AInventoryPlayerController::AInventoryPlayerController()
{
	InventoryManager = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("InventoryManager"));
	InventoryManager->SetIsReplicated(true);

	PlayerInventory = CreateDefaultSubobject<UInventoryEquipmentComponent>(TEXT("PlayerInventory"));
}

UInventoryManagerComponent* AInventoryPlayerController::GetInventoryManager() const noexcept
{
	check(InventoryManager);
	return InventoryManager;
}

UInventoryComponent* AInventoryPlayerController::GetInventory() const noexcept
{
	check(PlayerInventory);
	return PlayerInventory;
}

void AInventoryPlayerController::EnableUIControl()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetInputMode(FInputModeGameAndUI());
}

void AInventoryPlayerController::DisableUIControl()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	SetInputMode(FInputModeGameOnly());
}

void AInventoryPlayerController::BeginPlay()
{
	Super::BeginPlay();

	EnableUIControl();

	InventoryManager->SetInventoryComponent(PlayerInventory);
	InventoryManager->SetItemDataTable(ItemDataTable);
	PlayerInventory->SetPawn(GetPawn<AMultiplayerInventoryCharacter>());

	InventoryManager->Server_InitInventory();

	if (IsLocalController())
	{
		GameHUD = CastChecked<AGameHUD>(GetHUD());
	}
}
