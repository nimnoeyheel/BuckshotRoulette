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

	//UFUNCTION(NetMulticast, Reliable)
	//void Multicast_FireResult(int32 TargetPlayerIndex, EAmmoType FiredAmmo);

	UFUNCTION()
	void OnFireResult(int32 TargetPlayerIndex, EAmmoType FiredAmmo);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UInGameWidget> InGameWidgetClass;

	UPROPERTY()
	class UInGameWidget* InGameUI;
};
