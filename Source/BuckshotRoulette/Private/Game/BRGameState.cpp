// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/BRPlayerController.h"

void ABRGameState::SetTurnPlayer(APlayerState* NewTurnPlayer)
{
	if (HasAuthority())
	{
		TurnPlayer = NewTurnPlayer;
		OnRep_TurnPlayer();
	}
}

void ABRGameState::OnRep_TurnPlayer()
{
	// 모든 PlayerController에 턴 정보 업데이트 알림
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnTurnPlayerChanged();
		}
	}
	
	// UI에서 턴 변경 알림 처리
}

void ABRGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRGameState, TurnPlayer);
}
