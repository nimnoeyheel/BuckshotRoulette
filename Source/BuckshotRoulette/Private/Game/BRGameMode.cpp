// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameMode.h"
#include "Game/BRGameState.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "Types/AmmoType.h"

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
	InitMatchConfigs();
}

void ABRGameMode::TryStartGameIfReady()
{
	int ReadyCount = 0;

	// 모든 PlayerState 순회
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		ABRPlayerState* PS = PC ? Cast<ABRPlayerState>(PC->PlayerState) : nullptr;

		// 플레이어가 닉네임을 입력했으면
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

	CurrentMatchIdx = 0;
	CurrentRoundIdx = 0;

	// PlayerState HP Replicated 변수 초기화
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ABRPlayerState* BPS = Cast<ABRPlayerState>(PS))
		{
			BPS->Hp = AllMatches[CurrentMatchIdx].PlayerHP;
			BPS->OnRep_Hp(); // 서버의 UI도 업데이트해주기 위해 직접 호출
		}
	}

	SetupAmmoForRound(CurrentMatchIdx, CurrentRoundIdx);
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
		GS->SetTurnPlayer(Players[0]);
	}
}

void ABRGameMode::OnRoundEnd()
{
	UE_LOG(LogTemp, Log, TEXT("ABRGameMode::OnRoundEnd()"));

// 1. 플레이어들 HP 상태 체크
	// HP가 0인 플레이어 찾기
	TArray<ABRPlayerState*> BRPlayers;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ABRPlayerState* BPS = Cast<ABRPlayerState>(PS)) BRPlayers.Add(BPS);
	}

	ABRPlayerState* Winner = nullptr;
	ABRPlayerState* Loser = nullptr;
	if (BRPlayers.Num() == 2)
	{
		// 클라(1)의 체력이 0이라면 서버(0) 승
		if (BRPlayers[0]->Hp > 0 && BRPlayers[1]->Hp <= 0)
		{
			Winner = BRPlayers[0];
			Loser = BRPlayers[1];
		}

		// 서버(0)의 체력이 0이라면 클라(1) 승
		if (BRPlayers[1]->Hp > 0 && BRPlayers[0]->Hp <= 0)
		{
			Winner = BRPlayers[1];
			Loser = BRPlayers[0];
		}
	}

// 2. 만약 죽은 플레이어가 있다면 매치 종료/승수 관리
	if (Winner)
	{
		Winner->MatchWinCount++;
		UE_LOG(LogTemp, Log, TEXT("Player %s wins this match! [Total Wins: %d]"), *Winner->GetPlayerName(), Winner->MatchWinCount);

		// 최종 승리자 : 2선승에 도달하면 전체 게임 종료
		if (Winner->MatchWinCount >= 2)
		{
			// 최종 게임 오버 처리
			OnGameOver(Winner);
			UE_LOG(LogTemp, Log, TEXT("%s wins Game! [Total Wins: %d]"), *Winner->GetPlayerName(), Winner->MatchWinCount);
			return;
		}

		// 최종 승리자가 없다면 다음 매치로
		++CurrentMatchIdx;
		CurrentRoundIdx = 0;
		if (AllMatches.IsValidIndex(CurrentMatchIdx))
		{
			// 새 매치 HP/턴 리셋
			int32 StartHp = AllMatches[CurrentMatchIdx].PlayerHP;
			for (APlayerState* PS : GameState->PlayerArray)
			{
				if (ABRPlayerState* BPS = Cast<ABRPlayerState>(PS))
				{
					BPS->Hp = StartHp;
					BPS->OnRep_Hp();
				}
			}
			SetupAmmoForRound(CurrentMatchIdx, CurrentRoundIdx);
			UE_LOG(LogTemp, Log, TEXT("Start New Match_%d"), CurrentMatchIdx + 1);
		}
		return;
	}

// 3. 둘 다 살아있다면 다음 라운드로
	++CurrentRoundIdx;
	if (AllMatches[CurrentMatchIdx].Rounds.IsValidIndex(CurrentRoundIdx))
	{
		SetupAmmoForRound(CurrentMatchIdx, CurrentRoundIdx);
		// 라운드별 초기화
		UE_LOG(LogTemp, Log, TEXT("Start Next Round_%d"), CurrentRoundIdx + 1);
	}
}

void ABRGameMode::OnGameOver(class ABRPlayerState* Winner)
{
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ABRPlayerController* PC = Cast<ABRPlayerController>(PS->GetOwner()))
		{
			bool bIsWinner = (PS == Winner);
			
			UE_LOG(LogTemp, Log, TEXT("PS : %s"), *PS->GetPlayerName());
			UE_LOG(LogTemp, Log, TEXT("Winner : %s"), *Winner->GetPlayerName());

			ABRGameState* GS = Cast<ABRGameState>(GameState);
			if (GS) GS->Multicast_OnGameOver(Winner);
		}
	}
}

void ABRGameMode::InitMatchConfigs()
{
	AllMatches.Empty();

	// Match_1
	{
		FMatchConfig Match;
		Match.PlayerHP = 2;
		Match.Rounds = {
			{ 1, 2 },	// 1라운드 : 1 Live, 2 Blank
			{ 3, 2 },	// 2라운드 : 3 Live, 2 Blank
			{ 3, 2 }	// 3라운드 : 2 Live, 1 Blank
		};
		AllMatches.Add(Match);
	}

	// Match_2
	{
		FMatchConfig Match;
		Match.PlayerHP = 4;
		Match.Rounds = {
			{ 1, 1 },	// 1라운드 : 1 Live, 1 Blank
			{ 2, 2 },	// 2라운드 : 2 Live, 2 Blank
			{ 3, 3 }	// 3라운드 : 3 Live, 3 Blank
		};
		AllMatches.Add(Match);
	}

	// Match_3
	{
		FMatchConfig Match;
		Match.PlayerHP = 6;
		Match.Rounds = {
			{ 1, 2 },	// 1라운드 : 1 Live, 2 Blank
			{ 4, 4 },	// 2라운드 : 4 Live, 4 Blank
			{ 3, 2 }	// 3라운드 : 3 Live, 2 Blank
		};
		AllMatches.Add(Match);
	}
}

void ABRGameMode::SetupAmmoForRound(int32 MatchIdx, int32 RoundIdx)
{
	// AllMatches[MatchIdx].Rounds[RoundIdx]의 NumLive/NumBlank 값 이용
	if (!AllMatches.IsValidIndex(MatchIdx)) return;
	const FMatchConfig& Match = AllMatches[MatchIdx];
	if(!Match.Rounds.IsValidIndex(RoundIdx)) return;
	const FRoundAmmoConfig& Round = Match.Rounds[RoundIdx];

	// 실탄/공탄 배열 만들기
	TArray<EAmmoType> NewAmmo;
	for (int i = 0; i < Round.NumLive; ++i) NewAmmo.Add(EAmmoType::Live);
	for (int i = 0; i < Round.NumBlank; ++i) NewAmmo.Add(EAmmoType::Blank);

	// 랜덤 셔플
	for (int i = 0; i < NewAmmo.Num(); ++i)
	{
		int SwapIdx = FMath::RandRange(0, NewAmmo.Num()-1);
		NewAmmo.Swap(i, SwapIdx);
	}
	
	// GameState Replicated 변수 초기화
	ABRGameState* GS = GetGameState<ABRGameState>();
	GS->AmmoSequence = NewAmmo;
	GS->CurrentAmmoIndex = 0;
	GS->MatchIdx = MatchIdx + 1;
	GS->RoundIdx = RoundIdx + 1;
	GS->NumLive = Round.NumLive;
	GS->NumBlank = Round.NumBlank;

	// 서버의 UI도 업데이트해주기 위해 직접 호출
	GS->OnRep_UpdateNewRound();

	// 랜덤 장전 디버깅용
	for (int i = 0; i < GS->AmmoSequence.Num(); ++i)
	{
		FString AmmoTypeName;
		switch (GS->AmmoSequence[i])
		{
			case EAmmoType::Live:
				AmmoTypeName = TEXT("Live");
				break;
			case EAmmoType::Blank:
				AmmoTypeName = TEXT("Blank");
				break;
			default:
				AmmoTypeName = TEXT("Unknown");
				break;
		}
		FString msg = FString::Printf(TEXT("AmmoSequence[%d]: %s"), i, *AmmoTypeName);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);
	}
}
