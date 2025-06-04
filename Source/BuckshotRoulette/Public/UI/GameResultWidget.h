// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

// 승패 결과에 따른 위젯스위쳐
	UFUNCTION()
	void ShowResult(bool bIsWinner);

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	
// 메인으로 돌아가기, 게임 종료하기
	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnQuitClicked();

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;

// Winner Data
	UFUNCTION()
	void SetWinnerInfo(const FString& Winner, int32 ShotsFired, int32 ShellsEjected, int32 CigarettesSmoked, int32 MLOfBeerDrank, int32 TotalCash);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Winner;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_ShotsFired;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_ShellsEjected;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_CigarettesSmoked;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_MLOfBeerDrank;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_TotalCash;
	
// Loser Data
	UFUNCTION()
	void SetLoserNickname(const FString& Loser);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Loser;
	
};
