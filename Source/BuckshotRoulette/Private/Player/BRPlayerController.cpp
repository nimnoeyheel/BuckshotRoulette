// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "UI/NicknameEntryWidget.h"
#include "Game/BRGameState.h"
#include "Game/BRGameMode.h"
#include "UI/InGameWidget.h"

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

		if (InGameWidgetClass)
		{
			InGameUI = CreateWidget<UInGameWidget>(this, InGameWidgetClass);
			if (InGameUI)
			{
				InGameUI->AddToViewport();
			}
		}
	}

	// 마우스 커서 항상 표시
	bShowMouseCursor = true;

	// 마우스 이벤트 항상 활성화
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 입력 모드: UI+게임 모두 가능하게
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void ABRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Fire
	InputComponent->BindAction("Fire", IE_Pressed, this, &ABRPlayerController::TryFire);
}

void ABRPlayerController::SetInputEnable(bool bEnable)
{
	if (bEnable) EnableInput(this);
	else DisableInput(this);
}

void ABRPlayerController::OnTurnPlayerChanged()
{
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	ABRPlayerState* TurnPlayerState = Cast<ABRPlayerState>(GS->TurnPlayer);
	if (!GS || !TurnPlayerState || !InGameUI) return;

	// UI에 턴 플레이어 닉네임 업데이트
	FString Nick = TurnPlayerState->GetPlayerName();
	InGameUI->UpdateTurnNickname(Nick);

	// 내 턴인지 확인 후 입력 제어
	SetInputEnable(TurnPlayerState == PlayerState);
}

void ABRPlayerController::TryFire()
{
	// 해당 로직은 입력 제어를 추가했기 때문에 Fire 함수가 호출될 일이 없으므로 주석 처리
	// 내 턴이 아니면 리턴
	//ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	//if(!GS) return;
	//if(GS->TurnPlayer != PlayerState) return;

	// 서버에 RPC로 명령 전달
	ServerRPCFireActor();
}

void ABRPlayerController::ServerRPCFireActor_Implementation()
{
	// 총알 로직 발사

	// 성공 여부 판단 (액션 유효성 체크)
	// 턴 넘김
	ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->NextTurn();
	}
}
