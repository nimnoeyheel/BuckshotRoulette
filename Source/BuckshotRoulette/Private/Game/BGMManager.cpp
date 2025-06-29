// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BGMManager.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UBGMManager::UBGMManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<USoundBase> BGM0(TEXT("/Game/BuckShotRoulette/Audio/BGM_1.BGM_1"));
    static ConstructorHelpers::FObjectFinder<USoundBase> BGM1(TEXT("/Game/BuckShotRoulette/Audio/BGM_2.BGM_2"));
    static ConstructorHelpers::FObjectFinder<USoundBase> BGM2(TEXT("/Game/BuckShotRoulette/Audio/BGM_3.BGM_3"));
    static ConstructorHelpers::FObjectFinder<USoundBase> BGM3(TEXT("/Game/BuckShotRoulette/Audio/BGM_4.BGM_4"));
    static ConstructorHelpers::FObjectFinder<USoundBase> BGM4(TEXT("/Game/BuckShotRoulette/Audio/BGM_5.BGM_5"));

    if (BGM0.Succeeded()) BGMs.Add(BGM0.Object);
    if (BGM1.Succeeded()) BGMs.Add(BGM1.Object);
    if (BGM2.Succeeded()) BGMs.Add(BGM2.Object);
    if (BGM3.Succeeded()) BGMs.Add(BGM3.Object);
    if (BGM4.Succeeded()) BGMs.Add(BGM4.Object);

}

void UBGMManager::BeginPlay()
{
    Super::BeginPlay();

    // 컴포넌트가 시작되면 오디오 컴포넌트 생성
    AudioComp = NewObject<UAudioComponent>(this);
    if (AudioComp)
    {
        AudioComp->bAutoActivate = false;
        AudioComp->RegisterComponent();
    }
}

void UBGMManager::PlayBGM(int32 BGMIndex)
{
    if (!BGMs.IsValidIndex(BGMIndex)) return;
    if (BGMIndex == CurrentBGMIndex) return;

    CurrentBGMIndex = BGMIndex;

    if (AudioComp)
    {
        AudioComp->SetSound(BGMs[BGMIndex]);
        AudioComp->Play();
    }
}


