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
    // �Էµ� ���ڿ� ���� �˻�: �ʰ� �� �߶󳻱�
    if (Input.Len() > MaxNicknameLength)
    {
        Input = Input.Left(MaxNicknameLength);

        // �ؽ�Ʈ�ڽ� �� ���� ����
        EdtTxt_Nickname->SetText(FText::FromString(Input));
    }
    LastValidNickname = Input;
}

void UNicknameEntryWidget::OnEntryButtonClicked()
{
    FString Nick = EdtTxt_Nickname ? EdtTxt_Nickname->GetText().ToString() : TEXT("");
    // ���ڿ��� ���������
    if (Nick.IsEmpty())
    {
        // �����޼��� ǥ��
        Txt_ErrorMsg->SetVisibility(ESlateVisibility::Visible);
        // ī�޶� ����ŷ �߰��ϱ�
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->PlayerState)
    {
        ABRPlayerState* PS = Cast<ABRPlayerState>(PC->PlayerState);
        if (PS)
        {
            // ������ �г��� ����
            PS->ServerRPCSetPlayerName(LastValidNickname);
			SetVisibility(ESlateVisibility::Hidden);
        }
    }
}
