// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameMode.h"
#include "Game/BRGameState.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"

ABRGameMode::ABRGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> BRPlayerRef (TEXT("/Script/BuckshotRoulette.BRCharacter"));
	if (BRPlayerRef.Class)
	{
		DefaultPawnClass = BRPlayerRef.Class;
	}

	PlayerControllerClass = ABRPlayerController::StaticClass();
	PlayerStateClass = ABRPlayerState::StaticClass();
	GameStateClass = ABRGameState::StaticClass();
}