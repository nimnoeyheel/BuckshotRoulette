// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "UI/NicknameEntryWidget.h"

ABRPlayerController::ABRPlayerController()
{
	static ConstructorHelpers::FClassFinder<UNicknameEntryWidget> WidgetClass(TEXT("/Game/BuckShotRoulette/UI/WBP_NicknameEntry.WBP_NicknameEntry_C"));
	if (WidgetClass.Succeeded())
	{
		NicknameEntryWidgetClass = WidgetClass.Class;
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

		// 입장과 동시에 닉네임 설정하기
		if (NicknameEntryWidgetClass)
		{
			UNicknameEntryWidget* NickUI = CreateWidget<UNicknameEntryWidget>(this, NicknameEntryWidgetClass);
			if (NickUI)
			{
				NickUI->AddToViewport();
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
