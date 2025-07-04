﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/BRPlayerController.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ABRGameState::SetTurnPlayer(APlayerState* NewTurnPlayer)
{
	if (HasAuthority())
	{
		TurnPlayer = NewTurnPlayer;
		OnRep_TurnPlayer();
	}
}

void ABRGameState::Multicast_FireResult_Implementation(int32 FiringPlayerIndex, int32 TargetPlayerIndex, EAmmoType FiredAmmo, bool bIsLastAmmo)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC)
		{
			PC->OnFireResult(FiringPlayerIndex, TargetPlayerIndex, FiredAmmo, bIsLastAmmo);
		}
	}
}

void ABRGameState::OnRep_UpdateNewRound()
{
	// 모든 PlayerController에 새로운 라운드 정보 업데이트 알림
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			//PC->OnUpdateNickname();
			PC->OnUpdateNewRound();
		}
	}
}

void ABRGameState::Multicast_OnGameOver_Implementation(ABRPlayerState* Winner/*, bool bIsWinner*/)
{
	// 모든 PlayerController에 새로운 라운드 정보 업데이트 알림
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnGameOver(Winner);
		}
	}
}

void ABRGameState::OnRep_TurnPlayer()
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		//const FString RoleStr = PC->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT");
		//UE_LOG(LogTemp, Log, TEXT("GS::OnRep_TurnPlayer | PC Name is %s | ROLE = %s"), *PC->GetName(), *RoleStr);
		PC->OnTurnPlayerChanged();
	}
}

void ABRGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRGameState, TurnPlayer);
	DOREPLIFETIME(ABRGameState, MatchIdx);
	DOREPLIFETIME(ABRGameState, RoundIdx);
	DOREPLIFETIME(ABRGameState, NumLive);
	DOREPLIFETIME(ABRGameState, NumBlank);
}
