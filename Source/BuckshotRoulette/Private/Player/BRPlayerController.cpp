// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 서버-클라이언트 확인용
	if (IsLocalController())
	{
		FString ROLE = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
		UE_LOG(LogTemp, Log, TEXT("This is %s"), *ROLE);
	}
}
