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
#include "Actor/Shotgun.h"
#include "Actor/SlotComponent.h"
#include "Actor/Board.h"
#include "Actor/Item.h"

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

ABRPlayerState* ABRPlayerController::FindPlayerStateByIndex(int32 Index)
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS) return nullptr;

	for (APlayerState* PS : GS->PlayerArray)
	{
		if (ABRPlayerState* BPS = Cast<ABRPlayerState>(PS))
		{
			if (BPS->PlayerIndex == Index)
			{
				return BPS;
			}
		}
	}
	return nullptr;
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

	// 턴플레이어가 null이면 모두 활성화
	if (GS->TurnPlayer == nullptr) 
	{
		SetInputEnable(true);
		SetMouseInteractionEnable(true); // 마우스 입력도 강제로 활성화
		return;
	}

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

	ABRPlayerState* P1 = FindPlayerStateByIndex(1);
	ABRPlayerState* P2 = FindPlayerStateByIndex(2);

	FString Player1Nick = P1 ? P1->GetPlayerName() : TEXT("Unknown1");
	FString Player2Nick = P2 ? P2->GetPlayerName() : TEXT("Unknown2");

	UE_LOG(LogTemp, Warning, TEXT("OnUpdateNewRound: Player1[%s], Player2[%s]"), *Player1Nick, *Player2Nick);

	MainUI->InGameUI->UpdateNewRound(
		GS->MatchIdx, 
		GS->RoundIdx, 
		Player1Nick, 
		Player2Nick, 
		GS->NumLive, 
		GS->NumBlank
	);
}

void ABRPlayerController::OnUpdateHp()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !MainUI || !MainUI->InGameUI) return;

	TArray<APlayerState*> Players = GS->PlayerArray;

	ABRPlayerState* P1 = FindPlayerStateByIndex(1);
	ABRPlayerState* P2 = FindPlayerStateByIndex(2);

	int32 Player1Hp = P1 ? P1->Hp : -1;
	int32 Player2Hp = P2 ? P2->Hp : -1;

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
	ABRPlayerState* Attacker = Cast<ABRPlayerState>(PlayerState);
	if (!GS || !Attacker) return;

	if (GS->CurrentAmmoIndex >= GS->AmmoSequence.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("Ammo is null.."));
		return;
	}

	const EAmmoType FiredAmmo = GS->AmmoSequence[GS->CurrentAmmoIndex];

	// 디버깅용
	FString AmmoType;
	switch (FiredAmmo)
	{
		case EAmmoType::Live:
			AmmoType = TEXT("Live");
			break;
		case EAmmoType::Blank:
			AmmoType = TEXT("Blank");
			break;
		default:
			AmmoType = TEXT("Unknown");
			break;
	}
	UE_LOG(LogTemp, Log, TEXT("FiredAmmo is %s"), *AmmoType);

	// 다음 총알 미리 갱신
	GS->CurrentAmmoIndex++;

	// Winner Result Data 증가
	Attacker->ShotsFired++;
	Attacker->ShellsEjected++;
	Attacker->TotalCash += (Attacker->ShotsFired * 100) + (Attacker->ShellsEjected * 100);

	// Shotgun&Items 마우스 호버 제어
	SetMouseInteractionEnable(false);

	// FiringPlayerIndex 구하기
	const int32 FiringPlayerIndex = Attacker->PlayerIndex;
	ABRPlayerState* Target = FindPlayerStateByIndex(TargetPlayerIndex + 1);
	if (!Target) return;

	// 연출용: 발사 애니메이션은 여기서 서버가 발사자에게만 호출
	TriggerFireAnim(Attacker, TargetPlayerIndex);

	// 데미지 처리
	if (Target && FiredAmmo == EAmmoType::Live)
	{
		Target->Hp -= Attacker->IsKnifeEffectPending() ? 2 : 1;
		Target->OnRep_Hp();
	}
	// 나이프 사용 후 초기화
	Attacker->SetKnifeEffectPending(false);

	// 라운드 종료 조건 & 턴 유지 조건
	const bool bIsTargetDead = Target->Hp <= 0;
	const bool bIsLastAmmo = (GS->CurrentAmmoIndex >= GS->AmmoSequence.Num());
	const bool bIsSelfTarget = (FiringPlayerIndex == TargetPlayerIndex + 1);
	const bool bIsBlank = (FiredAmmo == EAmmoType::Blank);
	const bool bShouldEndRound = bIsTargetDead || bIsLastAmmo;
	const bool bShouldKeepTurnNextRound = bIsSelfTarget && bIsBlank;

	// 결과 연출 알림
	GS->Multicast_FireResult(FiringPlayerIndex, TargetPlayerIndex, FiredAmmo, bIsLastAmmo);

	// 라운드 흐름 제어 (서버 전용)
	if (bShouldEndRound)
	{
		OnRoundEnd();
	}
	else if (!bShouldKeepTurnNextRound && !TrySkipOpponentTurn(Attacker))
	{
		NextTurn();
	}
}

void ABRPlayerController::OnFireResult(int32 FiringPlayerIndex, int32 TargetPlayerIndex, EAmmoType FiredAmmo, bool bIsLastAmmo)
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !MainUI || !MainUI->InGameUI) return;

	ABRPlayerState* MyState = Cast<ABRPlayerState>(PlayerState);
	ABRPlayerState* TargetState = FindPlayerStateByIndex(TargetPlayerIndex + 1);
	if (!MyState || !TargetState) return;

	// 연출용: 내가 타겟일 때
	if (MyState == TargetState)
	{
		TriggerSelfFireAnim(FiringPlayerIndex, TargetPlayerIndex, MyState);

		if (FiredAmmo == EAmmoType::Live)
		{
			TriggerDamageAnim(MyState);
		}
		else if (FiredAmmo == EAmmoType::Blank)
		{
			// Blank 연출 필요 시 여기에 추가
		}
	}

	// 연출용: 상대가 타겟일 때
	else
	{
		TriggerSelfFireAnim(FiringPlayerIndex, TargetPlayerIndex, TargetState);

		if (FiredAmmo == EAmmoType::Live)
		{
			TriggerDamageAnim(TargetState);
		}
		else if (FiredAmmo == EAmmoType::Blank)
		{
			// Blank 연출 필요 시 여기에 추가
		}
	}
}

void ABRPlayerController::TriggerDamageAnim(ABRPlayerState* PS)
{
	APawn* MyPawn = PS->GetPawn();
	if (!MyPawn) return;
	ABRCharacter* Char = Cast<ABRCharacter>(MyPawn);
	if (!Char || !Char->GetShotgunActor()) return;

	Char->Multicast_TriggerDamageAnim();
}

void ABRPlayerController::TriggerDeathAnim(ABRPlayerState* PS)
{
	APawn* MyPawn = PS->GetPawn();
	if (!MyPawn) return;
	ABRCharacter* Char = Cast<ABRCharacter>(MyPawn);
	if (!Char || !Char->GetShotgunActor()) return;

	Char->Multicast_TriggerDeathAnim();
}

void ABRPlayerController::TriggerFireAnim(ABRPlayerState* PS, int32 TargetPlayerIndex)
{
	if(!PS) return;

	int32 FiringPlayerIndex = PS->PlayerIndex;

	if (FiringPlayerIndex != TargetPlayerIndex + 1)
	{
		if (APawn* MyPawn = GetPawn())
		{
			if (ABRCharacter* MyChar = Cast<ABRCharacter>(MyPawn))
			{
				MyChar->Multicast_TriggerAttackAnim();
			}

		}
	}
}

void ABRPlayerController::TriggerSelfFireAnim(int32 FiringPlayerIndex, int32 TargetPlayerIndex, ABRPlayerState* PS)
{
	APawn* MyPawn = PS->GetPawn();
	if (!MyPawn) return;
	ABRCharacter* Char = Cast<ABRCharacter>(MyPawn);
	if (!Char || !Char->GetShotgunActor()) return;

	if (FiringPlayerIndex == TargetPlayerIndex + 1)
	{
		bool bIsServer = PS->PlayerIndex == 1 ? true : false;
		Char->GetShotgunActor()->Multicast_TriggerSelfFireAnim(bIsServer);
	}
}

bool ABRPlayerController::TrySkipOpponentTurn(ABRPlayerState* PS)
{
	if (!HasAuthority()) return false;

	if (PS && PS->ShouldSkipOpponentTurn())
	{
		PS->SetSkipOpponentTurn(false); // 사용 후 초기화
		UE_LOG(LogTemp, Log, TEXT("Handcuff used. Keeping turn for %s."), *PS->GetPlayerName());

		// 상대방 수갑 애님 포커싱 연출
		return true;
	}
	return false;
}

void ABRPlayerController::NextTurn()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
			if (GM) GM->NextTurn();
		}
	), 3.f, false);
}

void ABRPlayerController::OnRoundEnd()
{
	if (!HasAuthority()) return;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			UE_LOG(LogTemp, Log, TEXT("This Round is Last!"));
			ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
			if (GM) GM->OnRoundEnd();
		}
	), 3.f, false);
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

void ABRPlayerController::SetMouseInteractionEnable(bool bEnabled)
{
	if (APawn* MyPawn = GetPawn())
	{
		ABRCharacter* MyChar = Cast<ABRCharacter>(MyPawn);
		if (MyChar && MyChar->GetShotgunActor())
		{
			if (AShotgun* Shotgun = Cast<AShotgun>(MyChar->GetShotgunActor()))
			{
				// 마우스 오버 잠금
				Shotgun->SetOwningCharacter(MyChar);
				Shotgun->Multicast_SetInteractionEnabled(bEnabled);
			}

			// Item 처리
			TArray<AItem*> MyItems;
			MyChar->GetOwnedItems(MyItems); // 소유한 아이템 리스트 가져오기

			for (AItem* Item : MyItems)
			{
				if (Item)
				{
					Item->Multicast_SetItemsInteractionEnabled(bEnabled);
				}
			}
		}
	}
}

void ABRPlayerController::ClientRPC_ShowCurrentAmmo_Implementation(EAmmoType AmmoType)
{
	FString TypeStr = (AmmoType == EAmmoType::Live) ? TEXT("Live")
					: (AmmoType == EAmmoType::Blank) ? TEXT("Blank")
					: TEXT("Unknown");

	if (MainUI && MainUI->InGameUI)
	{
		MainUI->InGameUI->ShowCurrentAmmoInfo(TypeStr);
	}
}

void ABRPlayerController::ServerRPC_ClickSlot_Implementation(USlotComponent* SlotComp, int32 SlotIdx)
{
	if (SlotComp && SlotComp->BoardOwner)
	{
		SlotComp->BoardOwner->OnSlotClicked(SlotComp, SlotIdx, this);
	}
}
