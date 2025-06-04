// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameResultWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UGameResultWidget::NativeConstruct()
{
	if (Btn_MainMenu) Btn_MainMenu->OnClicked.AddDynamic(this, &UGameResultWidget::OnMainMenuClicked);
	if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UGameResultWidget::OnQuitClicked);
}

void UGameResultWidget::ShowResult(bool bIsWinner)
{
	if (bIsWinner)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}
	else
	{
		WidgetSwitcher->SetActiveWidgetIndex(1);
	}
}

void UGameResultWidget::OnMainMenuClicked()
{
	// 메인 레벨로 트래블
	UE_LOG(LogTemp, Log, TEXT("Clicked The Main Menu Button."));
}

void UGameResultWidget::OnQuitClicked()
{
	// 게임 나가기
	UE_LOG(LogTemp, Log, TEXT("Clicked The Quit Button."));
}

void UGameResultWidget::SetWinnerInfo(const FString& Winner, int32 ShotsFired, int32 ShellsEjected, int32 CigarettesSmoked, int32 MLOfBeerDrank, int32 TotalCash)
{
	Txt_Winner->SetText(FText::FromString(Winner));
	Txt_ShotsFired->SetText(FText::AsNumber(ShotsFired));
	Txt_ShellsEjected->SetText(FText::AsNumber(ShellsEjected));
	Txt_CigarettesSmoked->SetText(FText::AsNumber(CigarettesSmoked));
	Txt_MLOfBeerDrank->SetText(FText::AsNumber(MLOfBeerDrank));
	Txt_TotalCash->SetText(FText::AsNumber(TotalCash));
}

void UGameResultWidget::SetLoserNickname(const FString& Loser)
{
	Txt_Loser->SetText(FText::FromString(Loser));
}
