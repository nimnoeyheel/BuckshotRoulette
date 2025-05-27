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

void ABRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetNumPlayers() > 2)
	{
		// 최대 인원 초과 시 퇴장
		NewPlayer->ClientTravel(TEXT("LV_MainMenu"), ETravelType::TRAVEL_Absolute);
	}

	// 플레이어 구분
	if (NewPlayer && NewPlayer->PlayerState)
	{
		ABRPlayerState* PS = Cast<ABRPlayerState>(NewPlayer->PlayerState);
		if (PS && PS->GetUniqueId().IsValid())
		{
			// 인덱스 부여
			PS->PlayerIndex = GetNumPlayers();
			UE_LOG(LogTemp, Log, TEXT("PlayerIndex: %d"), PS->PlayerIndex);

			// 고유 ID (스팀 연동 시 스팀ID 자동 할당)
			FString uniqueId = PS->GetUniqueId()->ToString();
			UE_LOG(LogTemp, Log, TEXT("Player UniqueId : %s"), *uniqueId);

			// 추후 둘 중 하나로 UI에 반영
		}
	}
}

void ABRGameMode::BeginPlay()
{
	Super::BeginPlay();

}
