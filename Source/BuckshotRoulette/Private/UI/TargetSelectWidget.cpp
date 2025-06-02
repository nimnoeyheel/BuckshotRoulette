// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TargetSelectWidget.h"
#include "Components/Button.h"
#include "Player/BRPlayerController.h"

void UTargetSelectWidget::NativeConstruct()
{
    if (Btn_Self) Btn_Self->OnClicked.AddDynamic(this, &UTargetSelectWidget::OnSelfClicked);
    if (Btn_Opponent) Btn_Opponent->OnClicked.AddDynamic(this, &UTargetSelectWidget::OnOpponentClicked);
}

void UTargetSelectWidget::OnSelfClicked()
{
    ABRPlayerController* PC = Cast<ABRPlayerController>(GetOwningPlayer());
    
    int32 TargetIdx = -1;
    if (PC->HasAuthority()) TargetIdx = 0; // 서버면 Self는 (0)
    else TargetIdx = 1;                    // 클라면 Self는 (1)

	OnTargetSelected.Broadcast(TargetIdx);
}

void UTargetSelectWidget::OnOpponentClicked()
{
    ABRPlayerController* PC = Cast<ABRPlayerController>(GetOwningPlayer());

    int32 TargetIdx = -1;
    if (PC->HasAuthority()) TargetIdx = 1; // 서버면 Opponent는 (1)
    else TargetIdx = 0;                    // 클라면 Opponent는 (0)

    OnTargetSelected.Broadcast(TargetIdx);
}