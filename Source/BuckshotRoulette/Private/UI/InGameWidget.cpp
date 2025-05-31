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
	Overlay_Ammo->SetVisibility(ESlateVisibility::Hidden);
	Overlay_Subtitle->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameWidget::HideNicknameEntryWidget()
{
    Overlay_NicknameEntry->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameWidget::UpdateTurnNickname(const FString& Nickname)
{
    if (Txt_TurnPlayerName) Txt_TurnPlayerName->SetText(FText::FromString(Nickname));
    Overlay_Turn->SetVisibility(ESlateVisibility::Visible);
}

void UInGameWidget::UpdateGameInfo(int32 MatchIdx, int32 RoundIdx, const FString& Player1Nick, const FString& Player2Nick, int32 HP, int32 NumLive, int32 NumBlank)
{
	Txt_MatchNum->SetText(FText::AsNumber(MatchIdx));
	Txt_RoundNum->SetText(FText::AsNumber(RoundIdx));
	Txt_Player1->SetText(FText::FromString(Player1Nick));
	Txt_Player2->SetText(FText::FromString(Player2Nick));
	Txt_Player1Hp->SetText(FText::AsNumber(HP));
	Txt_Player2Hp->SetText(FText::AsNumber(HP));

	FString AmmoInfo = FString::Printf(TEXT("%d Live, %d Blank"), NumLive, NumBlank);
	FString Msg = FString::Printf(TEXT("They enter the chamber in a hidden sequence."));

	Txt_AmmoInfo->SetText(FText::FromString(AmmoInfo));
	Txt_Subtitle->SetText(FText::FromString(Msg));

	Overlay_Ammo->SetVisibility(ESlateVisibility::Visible);
	Overlay_Subtitle->SetVisibility(ESlateVisibility::Visible);
}

