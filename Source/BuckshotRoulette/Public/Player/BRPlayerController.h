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
	// 턴에 따라 입력 제어 함수
	void SetInputEnable(bool bEnable);

	UFUNCTION()
	void OnTurnPlayerChanged();

	UFUNCTION()
	void OnUpdateGameInfo();

	// 발사 시스템
	UFUNCTION()
	void OnTargetSelected(int32 TargetPlayerIndex);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestFire(int32 TargetPlayerIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireResult(int32 TargetPlayerIndex, EAmmoType FiredAmmo);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UInGameWidget> InGameWidgetClass;

	UPROPERTY()
	class UInGameWidget* InGameUI;
};
