// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UFUNCTION()
	void ShowInGame();

	UFUNCTION()
	void ShowResult(bool bIsWinner);

	UPROPERTY(meta = (BindWidget))
	class UInGameWidget* InGameUI;

	UPROPERTY(meta = (BindWidget))
	class UGameResultWidget* ResultUI;

};
