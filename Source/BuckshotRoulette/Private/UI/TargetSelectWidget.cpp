// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TargetSelectWidget.h"
#include "Components/Button.h"

void UTargetSelectWidget::NativeConstruct()
{
    if (Btn_Self) Btn_Self->OnClicked.AddDynamic(this, &UTargetSelectWidget::OnSelfClicked);
    if (Btn_Opponent) Btn_Opponent->OnClicked.AddDynamic(this, &UTargetSelectWidget::OnOpponentClicked);
}

void UTargetSelectWidget::OnSelfClicked()
{
    OnTargetSelected.Broadcast(0); // 0: 자신
}

void UTargetSelectWidget::OnOpponentClicked()
{
    OnTargetSelected.Broadcast(1); // 1: 상대
}