// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetSelectWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetSelectedSignature, int32, TargetPlayerIndex);

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UTargetSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // 버튼 바인딩
    virtual void NativeConstruct() override;

    // 자신/상대 버튼 (디자이너에서 BindWidget)
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Self;
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Opponent;

    // UI에서 타겟 선택 시 델리게이트로 알림
    UPROPERTY(BlueprintAssignable, Category="Event")
    FTargetSelectedSignature OnTargetSelected;

    UFUNCTION()
    void OnSelfClicked();

    UFUNCTION()
    void OnOpponentClicked();
};
