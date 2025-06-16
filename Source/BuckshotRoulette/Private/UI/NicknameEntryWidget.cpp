// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NicknameEntryWidget.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"

void UNicknameEntryWidget::NativeConstruct()
{
    if (EdtTxt_Nickname)
    {
        EdtTxt_Nickname->OnTextChanged.AddDynamic(this, &UNicknameEntryWidget::OnNicknameTextChanged);
    }

	if (Btn_Entry)
	{
		Btn_Entry->OnClicked.AddDynamic(this, &UNicknameEntryWidget::OnEntryButtonClicked);
	}

    Txt_ErrorMsg->SetVisibility(ESlateVisibility::Hidden);
}

void UNicknameEntryWidget::OnNicknameTextChanged(const FText& Text)
{
    FString Input = Text.ToString();
    // 입력된 문자열 길이 검사: 초과 시 잘라내기
    if (Input.Len() > MaxNicknameLength)
    {
        Input = Input.Left(MaxNicknameLength);

        // 텍스트박스 값 강제 수정
        EdtTxt_Nickname->SetText(FText::FromString(Input));
    }
    LastValidNickname = Input;
}

void UNicknameEntryWidget::OnEntryButtonClicked()
{
    FString Nick = EdtTxt_Nickname ? EdtTxt_Nickname->GetText().ToString() : TEXT("");
    // 문자열이 비어있으면
    if (Nick.IsEmpty())
    {
        // 에러메세지 표시
        Txt_ErrorMsg->SetVisibility(ESlateVisibility::Visible);
        // 카메라 셰이킹 추가하기
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->PlayerState)
    {
        ABRPlayerState* PS = Cast<ABRPlayerState>(PC->PlayerState);
        if (PS)
        {
            // 서버로 닉네임 전달
            PS->ServerRPC_SetPlayerName(LastValidNickname);

            // 닉네임 완료 알림
            OnNicknameEntryComplete.Broadcast();
        }
    }
}
