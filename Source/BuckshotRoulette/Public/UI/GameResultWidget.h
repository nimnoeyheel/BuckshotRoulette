// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//virtual void NativeConstruct() override;
	UFUNCTION()
	void ShowResult(bool bIsWinner);

};
