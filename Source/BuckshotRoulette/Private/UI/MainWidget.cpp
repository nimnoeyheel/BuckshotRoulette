// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainWidget.h"
#include "Components/WidgetSwitcher.h"
#include "UI/InGameWidget.h"
#include "UI/GameResultWidget.h"

void UMainWidget::ShowInGame()
{
	MainSwitcher->SetActiveWidget(InGameUI);
}

void UMainWidget::ShowResult(bool bIsWinner)
{
	MainSwitcher->SetActiveWidget(ResultUI);
	ResultUI->ShowResult(bIsWinner);
}
