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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

#pragma region 턴 시스템
	// 턴 플레이어
	UPROPERTY(ReplicatedUsing=OnRep_TurnPlayer)
	class APlayerState* TurnPlayer;

	// 턴 변경시 클라에게 알림
	UFUNCTION()
	void OnRep_TurnPlayer();

	// 현재 턴 플레이어 세팅 (서버 only)
	void SetTurnPlayer(class APlayerState* NewTurnPlayer);

#pragma endregion
	
#pragma region 총알 시스템
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireResult(int32 FiringPlayerIndex, int32 TargetPlayerIndex, EAmmoType FiredAmmo, bool bIsLastAmmo);

	UPROPERTY(Replicated)
	TArray<EAmmoType> AmmoSequence;

	UPROPERTY(Replicated)
	int32 CurrentAmmoIndex = 0;
#pragma endregion

#pragma region InGameUI 반영을 위한 데이터 동기화
	// UI 업데이트를 위한 OnRep_함수
	UFUNCTION()
	void OnRep_UpdateNewRound();

	UPROPERTY(ReplicatedUsing = OnRep_UpdateNewRound)
	int32 MatchIdx;
	UPROPERTY(ReplicatedUsing = OnRep_UpdateNewRound)
	int32 RoundIdx;
	UPROPERTY(ReplicatedUsing = OnRep_UpdateNewRound)
	int32 NumLive;
	UPROPERTY(ReplicatedUsing = OnRep_UpdateNewRound)
	int32 NumBlank;
#pragma endregion

// 게임 종료 승패 UI Multicast
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnGameOver(class ABRPlayerState* Winner);

};
