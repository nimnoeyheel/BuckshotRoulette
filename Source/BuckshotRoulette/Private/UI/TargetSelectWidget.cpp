// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TargetSelectWidget.h"
#include "Components/Button.h"

void UTargetSelectWidget::NativeConstruct()
{
    if (Btn_Self) Btn_Self->OnClicked.AddDynamic(this, &UTargetSelectWidget::OnSelfClicked);
    if (Btn_Other) Btn_Other->OnClicked.AddDynamic(this, &UTargetSelectWidget::OnOtherClicked);
}

void UTargetSelectWidget::OnSelfClicked()
{
    OnTargetSelected.Broadcast(0); // 0: 자신
}

void UTargetSelectWidget::OnOtherClicked()
{
    OnTargetSelected.Broadcast(1); // 1: 상대
}