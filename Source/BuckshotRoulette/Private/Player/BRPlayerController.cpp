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
		return;
	}

	ABRPlayerState* TurnPlayerState = Cast<ABRPlayerState>(GS->TurnPlayer);
	if (!TurnPlayerState || !MainUI || !MainUI->InGameUI) return;

	// UI에 턴 플레이어 닉네임 업데이트
	FString Nick = TurnPlayerState->GetPlayerName();
	MainUI->InGameUI->UpdateTurnNickname(Nick);

	// 내 턴인지 확인 후 입력 제어
	bool bMyTurn = (TurnPlayerState == PlayerState);
	SetInputEnable(bMyTurn);
}

void ABRPlayerController::OnUpdateNewRound()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !MainUI || !MainUI->InGameUI) return;

	FString Player1Nick, Player2Nick;
	for (APlayerState* PS : GS->PlayerArray)
	{
		ABRPlayerState* BPS = Cast<ABRPlayerState>(PS);
		if (!BPS) continue;

		if (BPS->PlayerIndex == 1)
		{
			Player1Nick = BPS->GetPlayerName(); // 서버
		}
		else if (BPS->PlayerIndex == 2)
		{
			Player2Nick = BPS->GetPlayerName(); // 클라
		}
	}

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

	if (GS->CurrentAmmoIndex >= GS->AmmoSequence.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("Ammo is null.."));
		return;
	}

	bool bIsLastAmmo = (GS->CurrentAmmoIndex == GS->AmmoSequence.Num() - 1);
	UE_LOG(LogTemp, Log, TEXT("bIsLastAmmo = %s"), bIsLastAmmo ? TEXT("true") : TEXT("false"));

	EAmmoType FiredAmmo = GS->AmmoSequence[GS->CurrentAmmoIndex];

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
	PS->ShotsFired++;
	PS->ShellsEjected++;
	PS->TotalCash += (PS->ShotsFired * 100) + (PS->ShellsEjected * 100);

	// Shotgun&Items 마우스 호버 제어
	SetMouseInteractionEnable(false);

	// FiringPlayerIndex 구하기
	int32 FiringPlayerIndex = PS->PlayerIndex;
	// 공격 애니메이션
	TriggerFireAnim(PS, TargetPlayerIndex);

	// 결과를 모든 클라에 Multicast로 알림 => 서버의 클라2 PC와 클라2의 PC에서만 실행됨. 즉, 서버에서는 실행안됨.
	// 그래서 GameState에서 모든 PC를 순회하면서 함수를 직접 호출
	GS->Multicast_FireResult(FiringPlayerIndex, TargetPlayerIndex, FiredAmmo, bIsLastAmmo);
}

void ABRPlayerController::OnFireResult(int32 FiringPlayerIndex, int32 TargetPlayerIndex, EAmmoType FiredAmmo, bool bIsLastAmmo)
{
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

	// 내가 타겟일 때
	if (MyState == TargetState)
	{
		// Self Fire Anim
		TriggerSelfFireAnim(FiringPlayerIndex, TargetPlayerIndex, MyState);

		if (FiredAmmo == EAmmoType::Live)
		{
			AttackDamage(MyState);
			
			// Damage Anim
			TriggerDamageAnim(MyState);

			// 체력이 0이하라면
			if (MyState->Hp <= 0)
			{
				// 해당 라운드 종료
				bRoundOver = true;

				// Death Anim
				TriggerDeathAnim(MyState);
			}
			else if (HasAuthority())
			{
				// 내 턴에 나를 쐈다면
				if (MyState == GS->TurnPlayer)
				{
					if (!TrySkipOpponentTurn(MyState)) NextTurn();
				}
				// 상대가 나를 쐈다면
				else
				{
					if (!TrySkipOpponentTurn(OpponentState)) NextTurn();
				}
			}
		}
		else if (FiredAmmo == EAmmoType::Blank)
		{
			if (HasAuthority())
			{
				// 내 턴에 나를 쐈다면
				if (MyState == GS->TurnPlayer)
				{
					TrySkipOpponentTurn(MyState);

					// Knife 효과 무효: 변수 초기화
					MyState->SetKnifeEffectPending(false);
				}
				// 상대가 나를 쐈다면
				else
				{
					if (!TrySkipOpponentTurn(OpponentState))
					{
						NextTurn();

						// Knife 효과 무효: 변수 초기화
						OpponentState->SetKnifeEffectPending(false);
					}
				}
			}
		}
	}

	// 상대가 타겟일 때
	else if (OpponentState == TargetState)
	{
		TriggerSelfFireAnim(FiringPlayerIndex, TargetPlayerIndex, OpponentState);

		if (FiredAmmo == EAmmoType::Live)
		{
			AttackDamage(OpponentState);

			// Damage Anim
			TriggerDamageAnim(OpponentState);

			if (OpponentState->Hp <= 0)
			{
				// 해당 라운드 종료
				bRoundOver = true;

				// Death Anim
				TriggerDeathAnim(OpponentState);
			}
			else if (HasAuthority())
			{
				// 내 턴에 상대를 쐈다면
				if (MyState == GS->TurnPlayer)
				{
					if(!TrySkipOpponentTurn(MyState)) NextTurn();
				}
				// 상대가 자기 자신을 쐈으면
				else
				{
					if(!TrySkipOpponentTurn(OpponentState)) NextTurn();
				}
			}
		}
		else if (FiredAmmo == EAmmoType::Blank)
		{
			if (HasAuthority())
			{
				// 내 턴에 상대를 쐈다면
				if (MyState == GS->TurnPlayer)
				{
					if (!TrySkipOpponentTurn(MyState))
					{
						NextTurn();

						// Knife 효과 무효: 변수 초기화
						MyState->SetKnifeEffectPending(false);
					}
				}
				// 상대가 자기 자신을 쐈다면
				else
				{
					TrySkipOpponentTurn(OpponentState);

					// Knife 효과 무효: 변수 초기화
					OpponentState->SetKnifeEffectPending(false);
				}
			}
		}
	}
	
	// 마지막 총알일 때 해당 라운드 종료
	if (HasAuthority())
	{
		bool bIsAmmoEmpty = (GS->CurrentAmmoIndex >= GS->AmmoSequence.Num());
		if (bIsAmmoEmpty || bIsLastAmmo || bRoundOver)
		{
			bIsLastAmmo = false;
			OnRoundEnd();
		}
	}
}

void ABRPlayerController::AttackDamage(ABRPlayerState* PS)
{
	if (PS && PS->IsKnifeEffectPending())
	{
		PS->SetKnifeEffectPending(false); // 사용 후 초기화
		PS->Hp -= 2;
	}
	else PS->Hp--;

	OnUpdateHp();
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
