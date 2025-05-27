// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRCharacter.h"
#include "Player/BRPlayerState.h"

// Sets default values
ABRCharacter::ABRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ABRPlayerState* PS = GetPlayerState<ABRPlayerState>())
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerName: %s"), *PS->GetPlayerName());
	}

}

// Called every frame
void ABRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

