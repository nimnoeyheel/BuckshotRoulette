// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/ItemType.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void SetVisibleOverlay(UOverlay* Overlay, bool bVisible);

public:
// 게임 시작할 때 닉네임 입력
	UFUNCTION()
	void HideNicknameEntryWidget();

    UPROPERTY(meta = (BindWidget))
    class UNicknameEntryWidget* NicknameEntryWidget;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_NicknameEntry;
	
// 플레이어 닉네임
	//UFUNCTION()
	//void UpdatePlayerNickname(const FString& Player1Nick, const FString& Player2Nick);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Player1;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Player2;

// 턴 플레이어
	UFUNCTION()
	void UpdateTurnNickname(const FString& Nickname);

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_Turn;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_TurnPlayerName;

// 플레이어 HP
	UFUNCTION()
	void UpdatePlayerHp(int32 Player1Hp, int32 Player2Hp);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Player1Hp;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Player2Hp;

// 새로운 라운드 정보
	UFUNCTION()
	void UpdateNewRound(int32 MatchIdx, int32 RoundIdx, const FString& Player1Nick, const FString& Player2Nick, int32 NumLive, int32 NumBlank);

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_RoundInfo;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_MatchNum;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_RoundNum;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_AmmoInfo;
	
// 자막 오버레이
	UFUNCTION()
	void SetVisibleSubtitle(bool bVisible);

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_Subtitle;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Subtitle;

// 발사 시 타겟 선택
	UFUNCTION()
	void ShowFireRuleSubtitle();
	
	UFUNCTION()
	void ShowTargetSelectUI();

	UFUNCTION()
	void OnTargetSelected_Internal(int32 TargetPlayerIndex);

	UPROPERTY(meta = (BindWidget))
	class UTargetSelectWidget* TargetSelectWidget;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_TargetSelect;

// 아이템
	UFUNCTION()
	void ShowItemsRuleSubtitle(EItemType ItemType);

	UFUNCTION()
	void ShowCurrentAmmoInfo(const FString& AmmoType);

};
