// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Game/BRGameMode.h"

void ABRPlayerState::ServerRPC_SetPlayerName_Implementation(const FString& NewName)
{
	SetPlayerName(NewName);
	bNicknameEntered = true;

	// GameMode에서 서버-클라 모두 닉네임 입력했는지 확인 요청
	if (GetWorld())
	{
		ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->TryStartGameIfReady();
		}
	}
}

bool ABRPlayerState::ServerRPC_SetPlayerName_Validate(const FString& NewName)
{
	return true;
}

void ABRPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	UE_LOG(LogTemp, Log, TEXT("[CLIENT%d] Nickname: %s"), PlayerIndex , *GetPlayerName());
	// UI 업데이트
}

void ABRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRPlayerState, PlayerIndex);
}
