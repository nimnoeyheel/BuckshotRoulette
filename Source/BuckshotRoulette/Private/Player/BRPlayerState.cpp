// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABRPlayerState::ServerRPCSetPlayerName_Implementation(const FString& NewName)
{
	SetPlayerName(NewName);
}

bool ABRPlayerState::ServerRPCSetPlayerName_Validate(const FString& NewName)
{
	return true;
}

void ABRPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	UE_LOG(LogTemp, Log, TEXT("[CLIENT] Replicated Nickname: %s, PlayerIndex: %d"), *GetPlayerName(), PlayerIndex);
	// UI 업데이트
}

void ABRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRPlayerState, PlayerIndex);
}
