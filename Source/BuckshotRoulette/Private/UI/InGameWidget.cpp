// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "UI/NicknameEntryWidget.h"

void UInGameWidget::NativeConstruct()
{
	if (NicknameEntryWidget)
	{
		// 델리게이트 바인딩
		NicknameEntryWidget->OnNicknameEntryComplete.AddDynamic(this, &UInGameWidget::HideNicknameEntryWidget);
	}

	Overlay_Turn->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameWidget::HideNicknameEntryWidget()
{
    Overlay_NicknameEntry->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameWidget::UpdateTurnNickname(const FString& Nickname)
{
    if (Txt_Nickname) Txt_Nickname->SetText(FText::FromString(Nickname));
    Overlay_Turn->SetVisibility(ESlateVisibility::Visible);
}
