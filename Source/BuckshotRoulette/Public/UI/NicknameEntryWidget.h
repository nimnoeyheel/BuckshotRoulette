// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NicknameEntryWidget.generated.h"

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

    UFUNCTION()
    void OnNicknameTextChanged(const FText& Text);

    UFUNCTION()
    void OnEntryButtonClicked();

    // 원하는 최대 글자수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxNicknameLength = 10;

    FString LastValidNickname;
};
