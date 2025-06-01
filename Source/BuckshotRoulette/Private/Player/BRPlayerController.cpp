// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "UI/NicknameEntryWidget.h"
#include "Game/BRGameState.h"
#include "Game/BRGameMode.h"
#include "UI/InGameWidget.h"
#include "Types/AmmoType.h"
#include "GameFramework/PlayerState.h"

ABRPlayerController::ABRPlayerController()
{
	static ConstructorHelpers::FClassFinder<UInGameWidget> InGameUIRef(TEXT("/Game/BuckShotRoulette/UI/WBP_InGame.WBP_InGame_C"));
	if (InGameUIRef.Succeeded())
	{
		InGameWidgetClass = InGameUIRef.Class;
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

#pragma region UI
		// 레벨에 따라 UI 지정
		FString CurrentLevel = GetWorld()->GetMapName();
		CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // 레벨 이름 앞에 접두사 _ 제거
		if (CurrentLevel == "LV_Test")
		{
			// 인게임 UI
			if (InGameWidgetClass)
			{
				InGameUI = CreateWidget<UInGameWidget>(this, InGameWidgetClass);
				if (InGameUI) InGameUI->AddToViewport();
			}
		}
#pragma endregion
	}

	// 마우스 커서 항상 표시
	bShowMouseCursor = true;

	// 마우스 이벤트 항상 활성화
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 입력 모드: 시작할 때 UI만 가능하게 설정
	//FInputModeUIOnly InputMode;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void ABRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
}

void ABRPlayerController::SetInputEnable(bool bEnable)
{
	if (bEnable) EnableInput(this);
	else DisableInput(this);
}

void ABRPlayerController::OnTurnPlayerChanged()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS) return;

	ABRPlayerState* TurnPlayerState = Cast<ABRPlayerState>(GS->TurnPlayer);
	if (!TurnPlayerState || !InGameUI) return;

	// UI에 턴 플레이어 닉네임 업데이트
	FString Nick = TurnPlayerState->GetPlayerName();
	InGameUI->UpdateTurnNickname(Nick);

	// 내 턴인지 확인 후 입력 제어
	SetInputEnable(TurnPlayerState == PlayerState);
}

void ABRPlayerController::OnUpdateGameInfo()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || !InGameUI) return;

	// PlayerState에서 닉네임 가져오기 (자기 기준으로)
	TArray<APlayerState*> Players = GS->PlayerArray;
	FString Player1Nick = Players[0]->GetPlayerName();
	FString Player2Nick = Players[1]->GetPlayerName();

	InGameUI->UpdateGameInfo(GS->MatchIdx, GS->RoundIdx, Player1Nick, Player2Nick, GS->Hp, GS->NumLive, GS->NumBlank);
}

void ABRPlayerController::OnTargetSelected(int32 TargetPlayerIndex)
{
	// 해당 로직은 입력 제어를 추가했기 때문에 Fire 함수가 호출될 일이 없으므로 주석 처리
	// 내 턴이 아니면 리턴
	//ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	//if(!GS) return;
	//if(GS->TurnPlayer != PlayerState) return;

	// 서버에 RPC로 명령 전달
	
	ServerRPC_RequestFire(TargetPlayerIndex);
}

void ABRPlayerController::ServerRPC_RequestFire_Implementation(int32 TargetPlayerIndex)
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS) return;

	// 성공 여부 판단 (액션 유효성 체크)
	// 총알 소진 체크
	if (GS->CurrentAmmoIndex >= GS->AmmoSequence.Num()) return;

	EAmmoType FiredAmmo = GS->AmmoSequence[GS->CurrentAmmoIndex];
	GS->CurrentAmmoIndex++;

	// 결과를 모든 클라에 Multicast로 알림
	Multicast_FireResult(TargetPlayerIndex, FiredAmmo);

	// 턴 넘김
	ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->NextTurn();
	}
}

void ABRPlayerController::Multicast_FireResult_Implementation(int32 TargetPlayerIndex, EAmmoType FiredAmmo)
{
	// 타겟 플레이어의 HP 감소 / 사망, UI 이펙트, 사운드, 카메라 진동 등
	// (Client에서 Multicast 함수에서 처리)
}
