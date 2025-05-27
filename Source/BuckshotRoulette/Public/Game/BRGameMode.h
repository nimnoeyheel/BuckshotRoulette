// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BRGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ABRGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABRGameMode();

protected:
	virtual void PostLogin(class APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;

public:
	// 모든 플레이어가 닉네임 입력을 완료했는지 체크하는 함수
	void TryStartGameIfReady();

protected:
	void StartGame();
};
