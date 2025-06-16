// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "UI/NicknameEntryWidget.h"
#include "UI/TargetSelectWidget.h"
#include "Player/BRPlayerController.h"

void UInGameWidget::NativeConstruct()
{
	if (NicknameEntryWidget)
	{
		// 델리게이트 바인딩
		NicknameEntryWidget->OnNicknameEntryComplete.AddDynamic(this, &UInGameWidget::HideNicknameEntryWidget);
	}

	if (TargetSelectWidget)
	{
		// 델리게이트 바인딩
		TargetSelectWidget->OnTargetSelected.AddDynamic(this, &UInGameWidget::OnTargetSelected_Internal);
	}

	SetVisibleOverlay(Overlay_Turn, false);
	SetVisibleOverlay(Overlay_RoundInfo, false);
	SetVisibleOverlay(Overlay_Subtitle, false);
	SetVisibleOverlay(Overlay_TargetSelect, false);
}

void UInGameWidget::SetVisibleOverlay(UOverlay* Overlay, bool bVisible)
{
	if(bVisible) Overlay->SetVisibility(ESlateVisibility::Visible);
	else Overlay->SetVisibility(ESlateVisibility::Hidden);
}

void UInGameWidget::HideNicknameEntryWidget()
{
	SetVisibleOverlay(Overlay_NicknameEntry, false);
}

//void UInGameWidget::UpdatePlayerNickname(const FString& Player1Nick, const FString& Player2Nick)
//{
//	Txt_Player1->SetText(FText::FromString(Player1Nick));
//	Txt_Player2->SetText(FText::FromString(Player2Nick));
//}

void UInGameWidget::UpdateTurnNickname(const FString& Nickname)
{
	if (Txt_TurnPlayerName) Txt_TurnPlayerName->SetText(FText::FromString(Nickname));
	SetVisibleOverlay(Overlay_Turn, true);
}

void UInGameWidget::UpdatePlayerHp(int32 Player1Hp, int32 Player2Hp)
{
	Txt_Player1Hp->SetText(FText::AsNumber(Player1Hp));
	Txt_Player2Hp->SetText(FText::AsNumber(Player2Hp));
}

void UInGameWidget::UpdateNewRound(int32 MatchIdx, int32 RoundIdx, const FString& Player1Nick, const FString& Player2Nick, int32 NumLive, int32 NumBlank)
{
	Txt_MatchNum->SetText(FText::AsNumber(MatchIdx));
	Txt_RoundNum->SetText(FText::AsNumber(RoundIdx));
	Txt_Player1->SetText(FText::FromString(Player1Nick));
	Txt_Player2->SetText(FText::FromString(Player2Nick));

	FString AmmoInfo = FString::Printf(TEXT("%d Live, %d Blank"), NumLive, NumBlank);
	FString Msg = FString::Printf(TEXT("They enter the chamber in a hidden sequence."));

	Txt_AmmoInfo->SetText(FText::FromString(AmmoInfo));
	Txt_Subtitle->SetText(FText::FromString(Msg));

	SetVisibleOverlay(Overlay_RoundInfo, true);
	SetVisibleSubtitle(true);
}

void UInGameWidget::SetVisibleSubtitle(bool bVisible)
{
	if (bVisible)
	{
		SetVisibleOverlay(Overlay_Subtitle, true);

		// 1초 뒤 Hidden
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle,
			FTimerDelegate::CreateLambda([&]()
			{
				SetVisibleOverlay(Overlay_Subtitle, false);
			}
		), 3.0f, false);
	}
	else
	{
		SetVisibleOverlay(Overlay_Subtitle, false);
	}
}

void UInGameWidget::ShowFireRuleSubtitle()
{
	FString Msg = FString::Printf(TEXT("Shooting yourself with a blank skips the Opponent's Turn."));
	Txt_Subtitle->SetText(FText::FromString(Msg));

	SetVisibleOverlay(Overlay_Subtitle, true);
}

void UInGameWidget::ShowTargetSelectUI()
{
	SetVisibleOverlay(Overlay_TargetSelect, true);
}

void UInGameWidget::OnTargetSelected_Internal(int32 TargetPlayerIndex)
{
	SetVisibleOverlay(Overlay_TargetSelect, false);

	if (ABRPlayerController* PC = Cast<ABRPlayerController>(GetOwningPlayer()))
	{
		PC->OnTargetSelected(TargetPlayerIndex);
	}
}
