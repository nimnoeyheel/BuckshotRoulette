// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/AmmoType.h"
#include "GameFramework/PlayerController.h"
#include "BRPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ABRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABRPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
// 플레이어 인덱스 찾기
	ABRPlayerState* FindPlayerStateByIndex(int32 Index);

// 턴 시스템
	bool IsMyTurn();

	void SetInputEnable(bool bEnable);

	UFUNCTION()
	void OnTurnPlayerChanged();

// 새로운 라운드 정보 업데이트
	UFUNCTION()
	void OnUpdateNewRound();

// 플레이어 HP 업데이트
	UFUNCTION()
	void OnUpdateHp();

// 발사 시스템
	UFUNCTION()
	void OnTargetSelected(int32 TargetPlayerIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestFire(int32 TargetPlayerIndex);

	UFUNCTION()
	void OnFireResult(int32 FiringPlayerIndex, int32 TargetPlayerIndex, EAmmoType FiredAmmo, bool bIsLastAmmo);

// 애니메이션
	UFUNCTION()
	void TriggerFireAnim(class ABRPlayerState* PS, int32 TargetPlayerIndex, bool bIsLiveAmmo);

	UFUNCTION()
	void TriggerSelfFireAnim(int32 FiringPlayerIndex, int32 TargetPlayerIndex, class ABRPlayerState* PS, EAmmoType AmmoType);
	
	UFUNCTION()
	void TriggerDamageAnim(class ABRPlayerState* PS);
	
	UFUNCTION()
	void TriggerDeathAnim(class ABRPlayerState* PS);

// 턴, 라운드 변경 함수 타이머
	UFUNCTION()
	bool TrySkipOpponentTurn(class ABRPlayerState* PS);

	UFUNCTION()
	void NextTurn();
	
	UFUNCTION()
	void OnRoundEnd();

// 게임 종료
	UFUNCTION()
	void OnGameOver(class ABRPlayerState* Winner);

// 아이템 관련
	UFUNCTION(Server, Reliable)
	void ServerRPC_ClickSlot(class USlotComponent* SlotComp, int32 SlotIdx);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowCurrentAmmo(EAmmoType AmmoType);

// 마우스 호버 제어
	UFUNCTION()
	void SetMouseInteractionEnable(bool bEnabled);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UMainWidget> MainWidgetClass;

	UPROPERTY()
	class UMainWidget* MainUI;
};
