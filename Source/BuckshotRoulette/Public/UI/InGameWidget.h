// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void HideNicknameEntryWidget();

	UFUNCTION()
	void UpdateTurnNickname(const FString& Nickname);

    UPROPERTY(meta = (BindWidget))
    class UNicknameEntryWidget* NicknameEntryWidget;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_NicknameEntry;
	
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_Turn;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Nickname;
};
