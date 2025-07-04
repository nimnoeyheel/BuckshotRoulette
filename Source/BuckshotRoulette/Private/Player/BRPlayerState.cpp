﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BRPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Game/BRGameMode.h"
#include "Player/BRPlayerController.h"
#include "Game/BRGameState.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"

void ABRPlayerState::ServerRPC_SetPlayerName_Implementation(const FString& NewName)
{
	SetPlayerName(NewName);
	bNicknameEntered = true;

	UE_LOG(LogTemp, Warning, TEXT("Nickname set to %s"), *NewName);

	// GameMode에서 서버-클라 모두 닉네임 입력했는지 확인 요청
	ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
	if (GM) GM->TryStartGameIfReady();
}

bool ABRPlayerState::ServerRPC_SetPlayerName_Validate(const FString& NewName)
{
	return true;
}

void ABRPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

}

void ABRPlayerState::OnRep_Hp()
{
	// 모든 PlayerController에 알림 (각자 자기 화면에만 반영)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnUpdateHp();

			if (Hp <= 0)
			{
				PC->TriggerDeathAnim(this); // 죽는 연출은 여기서 해도 안전함
			}
		}
	}
}

void ABRPlayerState::OnRep_SkipOpponentTurn()
{
	//UE_LOG(LogTemp, Warning, TEXT("[OnRep] SkipOpponentTurn = %s"), bSkipOppenentTurn ? TEXT("TRUE") : TEXT("FALSE"));
	// 클라이언트에서도 수갑 애님 동기화
}

void ABRPlayerState::OnRep_KnifeEffectPending()
{
	//UE_LOG(LogTemp, Warning, TEXT("[OnRep] KnifeEffectPending = %s"), bKnifeEffectPending ? TEXT("TRUE") : TEXT("FALSE"));
	// 클라이언트에서도 나이프로 총 써는 애님 동기화
}

void ABRPlayerState::OnRep_TotalCash()
{

}

void ABRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABRPlayerState, PlayerIndex);
	DOREPLIFETIME(ABRPlayerState, Hp);
	DOREPLIFETIME(ABRPlayerState, MatchWinCount);
	DOREPLIFETIME(ABRPlayerState, ShotsFired);
	DOREPLIFETIME(ABRPlayerState, ShellsEjected);
	DOREPLIFETIME(ABRPlayerState, CigarettesSmoked);
	DOREPLIFETIME(ABRPlayerState, MLOfBeerDrank);
	DOREPLIFETIME(ABRPlayerState, TotalCash);

	DOREPLIFETIME(ABRPlayerState, bSkipOppenentTurn);
	DOREPLIFETIME(ABRPlayerState, bKnifeEffectPending);
}
