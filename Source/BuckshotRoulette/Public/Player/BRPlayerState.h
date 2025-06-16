// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BRPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ABRPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnRep_PlayerName() override;

	// 클라이언트에서 직접 닉네임 입력받을 때
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_SetPlayerName(const FString& NewName);

	// 플레이어 구분 인덱스 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
    int32 PlayerIndex = 0;

	// 닉네임을 설정했는지 여부
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bNicknameEntered = false;

// 플레이어 HP
	UFUNCTION()
	void OnRep_Hp();

	UPROPERTY(ReplicatedUsing = OnRep_Hp)
    int32 Hp = 0;

// 승수
	UPROPERTY(Replicated)
	int32 MatchWinCount = 0;

// 아이템
	void SetSkipOpponentTurn(bool b) { bSkipOppenentTurn = b; }
	bool ShouldSkipOpponentTurn() const { return bSkipOppenentTurn; }

	UPROPERTY(Replicated)
	bool bSkipOppenentTurn = false;

// WinnerData : 위너 UI만 반영
	UFUNCTION()
	void OnRep_TotalCash();

	UPROPERTY(Replicated)
	int32 ShotsFired = 0;

	UPROPERTY(Replicated)
	int32 ShellsEjected = 0;

	UPROPERTY(Replicated)
	int32 CigarettesSmoked = 0;

	UPROPERTY(Replicated)
	int32 MLOfBeerDrank = 0;

	UPROPERTY(ReplicatedUsing = OnRep_TotalCash)
	int32 TotalCash = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
