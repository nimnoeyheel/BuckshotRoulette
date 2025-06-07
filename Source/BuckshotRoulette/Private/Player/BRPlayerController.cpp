// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"
#include "UI/NicknameEntryWidget.h"
#include "Game/BRGameState.h"
#include "Game/BRGameMode.h"
#include "Types/AmmoType.h"
#include "GameFramework/PlayerState.h"
#include "UI/GameResultWidget.h"
#include "Character/BRCharacter.h"

ABRPlayerController::ABRPlayerController()
{
	static ConstructorHelpers::FClassFinder<UMainWidget> MainUIRef(TEXT("/Game/BuckShotRoulette/UI/WBP_Main.WBP_Main_C"));
	if (MainUIRef.Succeeded())
	{
		MainWidgetClass = MainUIRef.Class;
	}
}

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 서버-클라이언트 확인용
	if (IsLocalController())
	{
		FString ROLE = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
		UE_LOG(LogTemp, Log, TEXT("This is %s"), *ROLE);

		if (MainWidgetClass)
		{
			MainUI = CreateWidget<UMainWidget>(this, MainWidgetClass);
			if (MainUI)
			{
				MainUI->AddToViewport();
				MainUI->ShowInGame();
			}
		}

#pragma region UI
		//// 레벨에 따라 UI 지정
		//FString CurrentLevel = GetWorld()->GetMapName();
		//CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // 레벨 이름 앞에 접두사 _ 제거
		//if (CurrentLevel == "LV_Test")
		//{
		//	// 인게임 UI
		//	if (MainWidgetClass)
		//	{
		//		MainUI = CreateWidget<UMainWidget>(this, MainWidgetClass);
		//		if (MainUI)
		//		{
		//			MainUI->AddToViewport();
		//			MainUI->ShowInGame();
		//		}
		//	}
		//}
#pragma endregion
	}

	// 마우스 커서 항상 표시
	bShowMouseCursor = true;

	// 마우스 이벤트 항상 활성화
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 입력 모드: 시작할 때 UI만 가능하게 설정
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ABRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
}

bool ABRPlayerController::IsMyTurn()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !GS->TurnPlayer) return false;

	ABRPlayerState* TurnPlayerState = Cast<ABRPlayerState>(GS->TurnPlayer);
	if (!TurnPlayerState) return false;

	if (TurnPlayerState == PlayerState) return true;
	else return false;
}

void ABRPlayerController::SetInputEnable(bool bEnable)
{
	// 내 턴일 때
	if (bEnable)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	// 내 턴이 아닐 때
	else
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
}

void ABRPlayerController::OnTurnPlayerChanged()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS) return;

	ABRPlayerState* TurnPlayerState = Cast<ABRPlayerState>(GS->TurnPlayer);
	if (!TurnPlayerState || !MainUI || !MainUI->InGameUI) return;

	// UI에 턴 플레이어 닉네임 업데이트
	FString Nick = TurnPlayerState->GetPlayerName();
	MainUI->InGameUI->UpdateTurnNickname(Nick);

	// 내 턴인지 확인 후 입력 제어
	SetInputEnable(TurnPlayerState == PlayerState);
}

void ABRPlayerController::OnUpdateNewRound()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !MainUI || !MainUI->InGameUI) return;

	// PlayerState에서 닉네임 가져오기 (자기 기준으로)
	TArray<APlayerState*> Players = GS->PlayerArray;
	FString Player1Nick = Players[0]->GetPlayerName();
	FString Player2Nick = Players[1]->GetPlayerName();

	MainUI->InGameUI->UpdateNewRound(GS->MatchIdx, GS->RoundIdx, Player1Nick, Player2Nick, GS->NumLive, GS->NumBlank);
}

void ABRPlayerController::OnUpdateHp()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !MainUI || !MainUI->InGameUI) return;

	TArray<APlayerState*> Players = GS->PlayerArray;

	int32 Player1Hp = -1;
	int32 Player2Hp = -1;
	for(APlayerState* PS : Players)
	{
		ABRPlayerState* BPS = Cast<ABRPlayerState>(PS);
		if (BPS->PlayerIndex == 1) Player1Hp = BPS->Hp;
		if (BPS->PlayerIndex == 2) Player2Hp = BPS->Hp;
	}

	// 모든 플레이어의 HP UI 업데이트
	MainUI->InGameUI->UpdatePlayerHp(Player1Hp, Player2Hp);
}

void ABRPlayerController::OnTargetSelected(int32 TargetPlayerIndex)
{
	// 서버에 RPC로 명령 전달
	ServerRPC_RequestFire(TargetPlayerIndex);
}

void ABRPlayerController::ServerRPC_RequestFire_Implementation(int32 TargetPlayerIndex)
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	ABRPlayerState* PS = Cast<ABRPlayerState>(PlayerState);
	if (!GS || !PS) return;

	// 총알 소진 체크
	if (GS->CurrentAmmoIndex >= GS->AmmoSequence.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("Ammo is empty.."));
		return;
	}
	bool bIsLastAmmo = (GS->CurrentAmmoIndex == GS->AmmoSequence.Num() - 1);
	UE_LOG(LogTemp, Log, TEXT("bIsLastAmmo = %s"), bIsLastAmmo ? TEXT("true") : TEXT("false"));

	EAmmoType FiredAmmo = GS->AmmoSequence[GS->CurrentAmmoIndex];
	GS->CurrentAmmoIndex++;

	if (FiredAmmo == EAmmoType::Live)
	{
		UE_LOG(LogTemp, Warning, TEXT("FiredAmmo is Live"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FiredAmmo is Blank"));
	}

	// Winner Result Data 증가
	PS->ShotsFired++;
	PS->ShellsEjected++;
	PS->TotalCash += PS->ShotsFired + PS->ShellsEjected;

	// 공격 애니메이션
	// FiringPlayerIndex 구하기
	int32 FiringPlayerIndex = PS->PlayerIndex;
	if (PS->PlayerIndex == FiringPlayerIndex)
	{
		// 자기 자신에게 쏘면
		bool bSelfAttack = (FiringPlayerIndex == TargetPlayerIndex + 1);
		if (bSelfAttack)
		{
			// 총 애니메이션만 실행
		}
		else
		{
			if (APawn* MyPawn = GetPawn())
			{
				if (MyPawn)
				{
					ABRCharacter* MyChar = Cast<ABRCharacter>(MyPawn);
					if (MyChar) MyChar->Multicast_TriggerAttackAnim();
				}
			}
		}
	}

	// 결과를 모든 클라에 Multicast로 알림 => 서버의 클라2 PC와 클라2의 PC에서만 실행됨. 즉, 서버에서는 실행안됨.
	// 그래서 GameState에서 모든 PC를 순회하면서 함수를 직접 호출
	GS->Multicast_FireResult(TargetPlayerIndex, FiredAmmo, bIsLastAmmo);
}

void ABRPlayerController::OnFireResult(int32 TargetPlayerIndex, EAmmoType FiredAmmo, bool bIsLastAmmo)
{
	UE_LOG(LogTemp, Log, TEXT("ABRPlayerController::OnFireResult"));

	bool bRoundOver = false;

	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !MainUI || !MainUI->InGameUI) return;

	TArray<APlayerState*> Players = GS->PlayerArray;
	if (!Players.IsValidIndex(TargetPlayerIndex)) return;

	ABRPlayerState* MyState = Cast<ABRPlayerState>(PlayerState);
	ABRPlayerState* TargetState = Cast<ABRPlayerState>(Players[TargetPlayerIndex]);
	ABRPlayerState* OpponentState = nullptr;
	for (APlayerState* PS : Players)
	{
		if (PS != MyState) OpponentState = Cast<ABRPlayerState>(PS);
	}
	UE_LOG(LogTemp, Log, TEXT("TargetPlayer is %s"), *TargetState->GetPlayerName());

	// 내가 타겟일 때
	if (MyState == TargetState)
	{
		if (FiredAmmo == EAmmoType::Live)
		{
			MyState->Hp--;
			OnUpdateHp();
			UE_LOG(LogTemp, Log, TEXT("%s HP -1"), *MyState->GetPlayerName());

			// 체력이 0이하라면
			if (MyState->Hp <= 0)
			{
				// 해당 라운드 종료
				bRoundOver = true;
			}
			else if (HasAuthority())
			{
				ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
				if (GM) GM->NextTurn();
			}
		}
		else if (FiredAmmo == EAmmoType::Blank)
		{
			// 내 턴에 나를 쐈다면
			if (MyState == GS->TurnPlayer)
			{
				// 턴 유지
				UE_LOG(LogTemp, Log, TEXT("%s 턴 유지"), *MyState->GetPlayerName());
			}
			// 상대가 나를 쐈다면
			else if (HasAuthority())
			{
				// 턴 전환
				ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
				if (GM) GM->NextTurn();
			}
		}
	}

	// 상대가 타겟일 때
	else if (OpponentState == TargetState)
	{
		if (FiredAmmo == EAmmoType::Live)
		{
			OpponentState->Hp--;
			OnUpdateHp();
			UE_LOG(LogTemp, Log, TEXT("%s Damage -1"), *OpponentState->GetPlayerName());

			if (OpponentState->Hp <= 0)
			{
				// 해당 라운드 종료
				bRoundOver = true;
			}
			else if (HasAuthority())
			{
				ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
				if (GM) GM->NextTurn();
			}
		}
		else if (FiredAmmo == EAmmoType::Blank)
		{
			// 내 턴에 상대를 쐈다면
			if (MyState == GS->TurnPlayer)
			{
				if (HasAuthority())
				{
					ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
					if (GM) GM->NextTurn();
				}
			}
			// 상대가 자기 자신을 쐈다면
			else
			{
				// 턴 유지
				UE_LOG(LogTemp, Log, TEXT("%s 턴 유지"), *OpponentState->GetPlayerName());
			}
		}
	}

	// 마지막 총알일 때 해당 라운드 종료
	if ((bIsLastAmmo || bRoundOver) && HasAuthority())
	{
		bIsLastAmmo = false;
		UE_LOG(LogTemp, Log, TEXT("This Round is Last!"));
		ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) GM->OnRoundEnd();
	}
}

void ABRPlayerController::OnGameOver(ABRPlayerState* Winner)
{
	ABRPlayerState* PS = Cast<ABRPlayerState>(PlayerState);
	if (!PS || !MainUI || !MainUI->ResultUI) return;

	bool bIsWinner = (PS == Winner);

	if (bIsWinner)
	{
		MainUI->ResultUI->SetWinnerInfo(
			Winner->GetPlayerName(),
			PS->ShotsFired,
			PS->ShellsEjected,
			PS->CigarettesSmoked,
			PS->MLOfBeerDrank,
			PS->TotalCash
		);
	}
	else
	{
		MainUI->ResultUI->SetLoserNickname(PS->GetPlayerName());
	}
	MainUI->ShowResult(bIsWinner);
}
