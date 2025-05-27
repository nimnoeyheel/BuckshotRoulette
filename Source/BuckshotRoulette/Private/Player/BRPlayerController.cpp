// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerController.h"

void ABRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		FString ROLE = HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
		UE_LOG(LogTemp, Log, TEXT("This is %s"), *ROLE);
	}
}
