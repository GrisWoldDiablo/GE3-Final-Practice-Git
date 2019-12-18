// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FinalPracticeGameMode.h"
#include "FinalPracticeHUD.h"
#include "FinalPracticeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFinalPracticeGameMode::AFinalPracticeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFinalPracticeHUD::StaticClass();
}
