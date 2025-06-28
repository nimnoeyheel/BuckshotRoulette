// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameMode.h"
#include "Game/BRGameState.h"
#include "Actor/ItemBox.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "Types/AmmoType.h"
#include "Actor/Board.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Actor/SlotComponent.h"

ABRGameMode::ABRGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> BRPlayerRef(TEXT("/Script/BuckshotRoulette.BRCharacter"));
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

			RestartPlayer(NewPlayer);
		}
	}
}

void ABRGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitMatchConfigs();
}

AActor* ABRGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	 // 모든 PlayerStart 찾기
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	// 순서대로 할당
	int32 PlayerNum = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (It->Get() == Player)
		{
			break;
		}
		++PlayerNum;
	}

	if (PlayerStarts.IsValidIndex(PlayerNum))
	{
		return PlayerStarts[PlayerNum];
	}

	// 기본 랜덤
	return Super::ChoosePlayerStart_Implementation(Player);
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
		if (PS && PS->bNicknameEntered)
		{
			ReadyCount++;
		}
	}
	// 2명 모두 닉네임 입력 완료 시 선공 플레이어 랜덤 결정
	//if (ReadyCount == 2) PickFirstPlayer();
	if (ReadyCount == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Both players entered nickname. Starting game."));
		PickFirstPlayer();  // 이게 initializeGame() 호출함
	}
}

void ABRGameMode::PickFirstPlayer()
{
	// 연결된 모든 PlayerState 가져오기
	TArray<APlayerState*> AllPlayers = GameState->PlayerArray;

	int32 FirstIdx = FMath::RandRange(0, AllPlayers.Num() - 1);

	// 선공 플레이어 지정
	ABRGameState* GS = Cast<ABRGameState>(GameState);
	if (GS)
	{
		GS->SetTurnPlayer(AllPlayers[FirstIdx]);
		UE_LOG(LogTemp, Log, TEXT("TurnPlayer: %s"), *AllPlayers[FirstIdx]->GetPlayerName());

		// 턴이 정해지면 게임 시작
		initializeGame();
	}
}

void ABRGameMode::initializeGame()
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
	
	// 슬롯 오너 설정
	InitSlotOwners();

	StartRound(CurrentMatchIdx, CurrentRoundIdx);
}

void ABRGameMode::StartRound(int32 MatchIdx, int32 RoundIdx)
{
	// 총알 설정
	SetupAmmoForRound(MatchIdx, RoundIdx);

	// 아이템 설정
	SetupItemsForRound(MatchIdx, RoundIdx);

	// 매치2부터 아이템 시스템 시작
	if (MatchIdx >= 1)
	{
		// 턴플레이어는 null로 초기화(아이템 준비 끝날때까지)
		ResetTurnForItemPhase();

		NumActiveItemBoxes = 0;

		TArray<APlayerController*> PCs;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (!PC) continue;
			PCs.Add(PC);

			FVector SpawnLoc = (PCs.Num() == 1)
				? FVector(750, 10, 225)		// 서버 (X=750.000000,Y=10.000000,Z=225.000000)
				: FVector(220, 10, 225);	// 클라 (X=220.000000,Y=10.000000,Z=225.000000)

			FTransform SpawnTransform = FTransform(FRotator(0), SpawnLoc);
			FActorSpawnParameters Params;
			Params.Owner = PC;
			ItemBox = GetWorld()->SpawnActor<AItemBox>(AItemBox::StaticClass(), SpawnTransform, Params);

			if (ItemBox)
			{
				ItemBox->SetOwningPlayer(PC);
				ItemBox->SetBoardOwner(BoardActor);
				ItemBox->InitPendingItems(CurrentRoundItems);
				ItemBox->SetReplicates(true);
				NumActiveItemBoxes++; // 박스 스폰 시마다 1씩 증가
			}
		}
	}
}

void ABRGameMode::NextTurn()
{
	ABRGameState* GS = Cast<ABRGameState>(GameState);
	if (!GS) return;

	// 현재 턴플레이어 저장
	if (GS->TurnPlayer) LastTurnPlayer = GS->TurnPlayer;

	// 턴 넘기기
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
		if (AllMatches.IsValidIndex(CurrentMatchIdx + 1))
		{
			++CurrentMatchIdx;
			CurrentRoundIdx = 0;
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
			StartRound(CurrentMatchIdx, CurrentRoundIdx);
			UE_LOG(LogTemp, Log, TEXT("Start New Match_%d"), CurrentMatchIdx + 1);
		}
		return;
	}

// 3. 둘 다 살아있다면 다음 라운드로
	if (AllMatches[CurrentMatchIdx].Rounds.IsValidIndex(CurrentRoundIdx + 1))
	{
		++CurrentRoundIdx;
		StartRound(CurrentMatchIdx, CurrentRoundIdx);
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

void ABRGameMode::ResetTurnForItemPhase()
{
	if (ABRGameState* GS = GetGameState<ABRGameState>())
	{
		GS->SetTurnPlayer(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("TurnPlayer Reset for Item Setup Phase"));
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
			{ 3, 3 },	// 3라운드 : 3 Live, 3 Blank
			{ 4, 4 },	// 4라운드 : 4 Live, 4 Blank
			{ 5, 5 }	// 5라운드 : 5 Live, 5 Blank
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
			{ 3, 2 },	// 3라운드 : 3 Live, 2 Blank
			{ 4, 4 },	// 4라운드 : 4 Live, 4 Blank
			{ 5, 5 }	// 5라운드 : 5 Live, 5 Blank
		};
		AllMatches.Add(Match);
	}
}

void ABRGameMode::SetupAmmoForRound(int32 MatchIdx, int32 RoundIdx)
{
	// AllMatches[MatchIdx].Rounds[RoundIdx]의 NumLive/NumBlank 값 이용
	if (!AllMatches.IsValidIndex(MatchIdx)) return;
	const FMatchConfig& Match = AllMatches[MatchIdx];
	if (!Match.Rounds.IsValidIndex(RoundIdx)) return;
	const FRoundAmmoConfig& Round = Match.Rounds[RoundIdx];

	// 실탄/공탄 배열 만들기
	TArray<EAmmoType> NewAmmo;
	for (int i = 0; i < Round.NumLive; ++i) NewAmmo.Add(EAmmoType::Live);
	for (int i = 0; i < Round.NumBlank; ++i) NewAmmo.Add(EAmmoType::Blank);

	// 랜덤 셔플
	for (int i = 0; i < NewAmmo.Num(); ++i)
	{
		int SwapIdx = FMath::RandRange(0, NewAmmo.Num() - 1);
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

void ABRGameMode::SetupItemsForRound(int32 MatchIdx, int32 RoundIdx)
{
	CurrentRoundItems.Empty();

	// 매치1은 아이템 X
	if (MatchIdx == 0) return;

	// 매치2부터 아이템 시스템 적용
	if (MatchIdx == 1) // 매치2
	{
		if (RoundIdx == 0)
		{
			// 1 Round : 담배, 수갑
			CurrentRoundItems.Add(EItemType::Cigarette);
			CurrentRoundItems.Add(EItemType::Handcuff);
		}
		else if (RoundIdx == 1)
		{
			// 2 Round : 돋보기, 맥주, 칼
			CurrentRoundItems.Add(EItemType::Magnifier);
			CurrentRoundItems.Add(EItemType::Beer);
			CurrentRoundItems.Add(EItemType::Knife);
		}
		else if (RoundIdx >= 2)
		{
			// 3 Round ~ : 아이템 3종 랜덤 지급
			TArray<EItemType> Pool = {
				EItemType::Cigarette,
				EItemType::Handcuff,
				EItemType::Magnifier,
				EItemType::Beer,
				EItemType::Knife
			};
			for (int i = 0; i < 3; ++i)
			{
				int32 idx = FMath::RandRange(0, Pool.Num() - 1);
				CurrentRoundItems.Add(Pool[idx]);
				Pool.RemoveAt(idx); // 중복 방지
			}
		}
	}
	else if (MatchIdx == 2) // 매치3, 모든 라운드 아이템 3종 랜덤 지급
	{
		TArray<EItemType> Pool = {
				EItemType::Cigarette,
				EItemType::Handcuff,
				EItemType::Magnifier,
				EItemType::Beer,
				EItemType::Knife
		};

		for (int i = 0; i < 3; ++i)
		{
			int32 idx = FMath::RandRange(0, Pool.Num() - 1);
			CurrentRoundItems.Add(Pool[idx]);
			Pool.RemoveAt(idx); // 중복 방지
		}
	}
}

void ABRGameMode::InitSlotOwners()
{
	for (TActorIterator<ABoard> BoardIt(GetWorld()); BoardIt; ++BoardIt)
	{
		SetBoardOwner(*BoardIt);
		break;
	}

	if (!BoardActor || BoardActor->SlotComponents.Num() < 16) return;

	// 플레이어컨트롤러 배열
	TArray<APlayerState*> PSs;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (PS) PSs.Add(PS);
	}

	// 0~7: 서버, 8~15: 클라
	for (int i = 0; i < 8; ++i)
	{
		BoardActor->SetSlotOwner(i, PSs[0]);
		BoardActor->OnRep_SlotOwners();
	}
	for (int i = 8; i < 16; ++i)
	{
		BoardActor->SetSlotOwner(i, PSs[1]);
		BoardActor->OnRep_SlotOwners();
	}
}

void ABRGameMode::NotifyItemBoxDestroyed()
{
	// NumActiveItemBoxes = FMath::Max(NumActiveItemBoxes - 1, 0);
	NumActiveItemBoxes--;
	if (NumActiveItemBoxes > 0) return;

	// 모두 소멸: 본격 라운드 시작
	DecideTurnAfterItemSetup();
}

void ABRGameMode::DecideTurnAfterItemSetup()
{
	ABRGameState* GS = GetGameState<ABRGameState>();
	if (!GS) return;

	TArray<APlayerState*> AllPlayers = GS->PlayerArray;
	if (AllPlayers.Num() < 2) return;

	int32 StartIdx = 0;

	// LastTurnPlayer가 nullptr가 아니라면 다음 인덱스를 찾음
	if (LastTurnPlayer)
	{
		int32 LastIdx = AllPlayers.IndexOfByKey(LastTurnPlayer);
		StartIdx = (LastIdx + 1) % AllPlayers.Num();
	}

	GS->SetTurnPlayer(AllPlayers[StartIdx]);
}
