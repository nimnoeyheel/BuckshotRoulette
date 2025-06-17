// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/ItemType.h"
#include "BRGameMode.generated.h"

USTRUCT(BlueprintType)
struct FRoundAmmoConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 NumLive = 0;

	UPROPERTY(EditAnywhere)
	int32 NumBlank = 0;
};

USTRUCT(BlueprintType)
struct FMatchConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FRoundAmmoConfig> Rounds;

	UPROPERTY(EditAnywhere)
	int32 PlayerHP; // Match별 HP 세팅용
};

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ABRGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABRGameMode();

protected:
	virtual void PostLogin(class APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

public:

#pragma region 턴&게임 시스템
	// 모든 플레이어가 닉네임 입력을 완료했는지 체크하는 함수
	void TryStartGameIfReady();

	// 턴 초기화
	void PickFirstPlayer();
	void initializeGame();

	// 라운드 시작
	void StartRound(int32 MatchIdx, int32 RoundIdx);

	// 턴 플레이어가 액션을 마쳤을 때 서버에서 호출
	void NextTurn();

	// 라운드 종료
	void OnRoundEnd();
	
	// 게임 종료
	void OnGameOver(class ABRPlayerState* Winner);

	// 마지막 턴 플레이어 저장
	UPROPERTY()
	APlayerState* LastTurnPlayer = nullptr;
#pragma endregion

#pragma region 총알 시스템
	// 매치-라운드 별 HP와 총알 갯수 명시
	void InitMatchConfigs();

	// 장전 시 실탄, 공탄 랜덤 장전
	void SetupAmmoForRound(int32 MatchIdx, int32 RoundIdx);

	TArray<FMatchConfig> AllMatches;

	int32 CurrentMatchIdx = 0;
	int32 CurrentRoundIdx = 0;
#pragma endregion

#pragma region 아이템 시스템
	void SetupItemsForRound(int32 MatchIdx, int32 RoundIdx);

	void InitSlotOwners();

	void SetBoardOwner(class ABoard* InBoard) { BoardActor = InBoard; }
	class ABoard* GetBoardOwner() const { return BoardActor; }

	UPROPERTY()
	class ABoard* BoardActor;

	UPROPERTY()
	class AItemBox* ItemBox;

	UPROPERTY()
	TArray<EItemType> CurrentRoundItems;

	void NotifyItemBoxDestroyed();
	void DecideTurnAfterItemSetup();

	UPROPERTY()
	int32 NumActiveItemBoxes = 0;
#pragma endregion

};
