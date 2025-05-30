// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Types/AmmoType.h"
#include "BRGameState.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ABRGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	// 턴 플레이어
	UPROPERTY(ReplicatedUsing=OnRep_TurnPlayer)
	class APlayerState* TurnPlayer;

	// 턴 변경시 클라에게 알림
	UFUNCTION()
	void OnRep_TurnPlayer();

	// 현재 턴 플레이어 세팅 (서버 only)
	void SetTurnPlayer(class APlayerState* NewTurnPlayer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	
	// 총알 시스템
	UPROPERTY(Replicated)
	TArray<EAmmoType> AmmoSequence;

	UPROPERTY(Replicated)
	int32 CurrentAmmoIndex = 0;
};
