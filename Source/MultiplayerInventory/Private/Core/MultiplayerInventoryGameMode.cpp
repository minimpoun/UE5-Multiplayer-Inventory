// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerInventoryGameMode.h"
#include "MultiplayerInventoryCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayerInventoryGameMode::AMultiplayerInventoryGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
