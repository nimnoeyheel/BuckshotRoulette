// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainWidget.h"
#include "Components/WidgetSwitcher.h"
#include "UI/InGameWidget.h"
#include "UI/GameResultWidget.h"

void UMainWidget::ShowInGame()
{
	MainSwitcher->SetActiveWidgetIndex(0);
}

void UMainWidget::ShowResult(bool bIsWinner)
{
	MainSwitcher->SetActiveWidgetIndex(1);
	ResultUI->ShowResult(bIsWinner);
}
