// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BGMManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUCKSHOTROULETTE_API UBGMManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBGMManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
    void PlayBGM(int32 BGMIndex);

protected:
    UPROPERTY(EditDefaultsOnly, Category="BGM")
    TArray<USoundBase*> BGMs;

    UPROPERTY()
    class UAudioComponent* AudioComp;

	// 중복 재생 방지용
	int32 CurrentBGMIndex = -1;
		
};
