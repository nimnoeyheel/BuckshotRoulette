// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NicknameEntryWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNicknameEntryComplete);

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UNicknameEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UEditableText* EdtTxt_Nickname;

    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Entry;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Txt_ErrorMsg;

    // 닉네임 입력 완료 알림 델리게이트
    UPROPERTY(BlueprintAssignable, Category="Event")
    FOnNicknameEntryComplete OnNicknameEntryComplete;

    UFUNCTION()
    void OnNicknameTextChanged(const FText& Text);

    UFUNCTION()
    void OnEntryButtonClicked();

    // 원하는 최대 글자수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxNicknameLength = 10;

    FString LastValidNickname;
};
