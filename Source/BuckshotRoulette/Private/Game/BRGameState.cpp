// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BRGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/BRPlayerController.h"
#include "UI/InGameWidget.h"

void ABRGameState::SetTurnPlayer(APlayerState* NewTurnPlayer)
{
	if (HasAuthority())
	{
		TurnPlayer = NewTurnPlayer;
		OnRep_TurnPlayer();
	}
}

void ABRGameState::Multicast_FireResult_Implementation(int32 TargetPlayerIndex, EAmmoType FiredAmmo)
{

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC)
		{
			PC->OnFireResult(TargetPlayerIndex, FiredAmmo);
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
			PC->OnUpdateNewRound();
		}
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
