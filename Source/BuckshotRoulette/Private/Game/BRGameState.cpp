// Fill out your copyright notice in the Description page of Project Settings.


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

void ABRGameState::RemoveNextAmmo()
{
	// 삭제 전 로그출력
	for (int i = 0; i < AmmoSequence.Num(); ++i)
	{
		FString AmmoTypeName;
		switch (AmmoSequence[i])
		{
			case EAmmoType::Live:
				AmmoTypeName = TEXT("Live");
				break;
			case EAmmoType::Blank:
				AmmoTypeName = TEXT("Blank");
				break;
			default:
				break;
		}
		FString msg = FString::Printf(TEXT("AmmoSequence[%d]: %s"), i, *AmmoTypeName);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);
	}

	if (AmmoSequence.IsValidIndex(CurrentAmmoIndex))
	{
		AmmoSequence.RemoveAt(CurrentAmmoIndex);
		CurrentAmmoIndex = FMath::Clamp(CurrentAmmoIndex, 0, AmmoSequence.Num() - 1);

		// 삭제 후 로그출력
		for (int i = 0; i < AmmoSequence.Num(); ++i)
		{
			FString AmmoTypeName;
			switch (AmmoSequence[i])
			{
				case EAmmoType::Live:
					AmmoTypeName = TEXT("Live");
					break;
				case EAmmoType::Blank:
					AmmoTypeName = TEXT("Blank");
					break;
				default:
					break;
			}
			FString msg = FString::Printf(TEXT("AmmoSequence[%d]: %s"), i, *AmmoTypeName);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);
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
	// 모든 PlayerController에 턴 정보 업데이트 알림
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABRPlayerController* PC = Cast<ABRPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnTurnPlayerChanged();
		}
	}

	/*ABRPlayerController* PC = Cast<ABRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->OnTurnPlayerChanged();
	}*/
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
