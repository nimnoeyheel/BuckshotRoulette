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

void ABRGameMode::TryStartGameIfReady()
{
	int ReadyCount = 0;

	// 모든 PlayerState 순회
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		ABRPlayerState* PS = PC ? Cast<ABRPlayerState>(PC->PlayerState) : nullptr;

		// 플레이어 닉네임이 비어있지 않으면 카운트
		//if (PS && !PS->GetPlayerName().IsEmpty())
		if(PS && PS->bNicknameEntered)
		{
			ReadyCount++;
		}
	}

	// 2명 모두 닉네임 입력 완료 시 선공 플레이어 랜덤 결정
	if(ReadyCount == 2) PickFirstPlayer();
}

void ABRGameMode::PickFirstPlayer()
{
	// 연결된 모든 PlayerState 가져오기
	TArray<APlayerState*> AllPlayers = GameState->PlayerArray;

	int32 FirstIdx = FMath::RandRange(0, AllPlayers.Num() -1);

	// 선공 플레이어 지정
	ABRGameState* GS = Cast<ABRGameState>(GameState);
	if (GS)
	{
		GS->SetTurnPlayer(AllPlayers[FirstIdx]);

		UE_LOG(LogTemp, Log, TEXT("TurnPlayer: %s"), *AllPlayers[FirstIdx]->GetPlayerName());

		// 턴이 정해지면 게임 시작
		StartGame();
	}
}

void ABRGameMode::StartGame()
{
	// UI, 게임 보드/상태 초기화, 첫 턴 알림 등
	UE_LOG(LogTemp, Log, TEXT("Game Start!"));

	// UI 처리 - 턴 알림

}

void ABRGameMode::NextTurn()
{
	ABRGameState* GS = Cast<ABRGameState>(GameState);
	if(!GS) return;

	TArray<APlayerState*> Players = GameState->PlayerArray;
	if (GS->TurnPlayer == Players[0])
	{
		GS->SetTurnPlayer(Players[1]);
	}
	else
	{
		GS->SetTurnPlayer(Players[1]);
	}

	// 3인 이상 시
	//int CurrentIdx = GameState->PlayerArray.IndexOfByKey(GS->TurnPlayer);
	//int NextIdx = (CurrentIdx + 1) % GameState->PlayerArray.Num();
	//GS->SetTurnPlayer(GameState->PlayerArray[NextIdx]);
}
