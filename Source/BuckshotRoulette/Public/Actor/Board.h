// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ItemType.h"
#include "Board.generated.h"

UCLASS()
class BUCKSHOTROULETTE_API ABoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Shotgun
    AActor* GetShotgunActor() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* GunAttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChildActorComponent* ShotgunChild;

// Board
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BoardMeshComp;

// Slot & Item
	// 슬롯 클릭 이벤트
	void OnSlotClicked(class USlotComponent* Slot, int32 SlotIdx, class APlayerController* RequestingPlayer);

	// 아이템 스폰
	void SpawnItem(EItemType ItemType, class APlayerController* ForPlayer, bool _bIsLastItem);

	// 플레이어별 PendingItem 관리 (서버만)
	UPROPERTY()
	TMap<APlayerController*, class AItem*> PendingItems;

	UPROPERTY()
	TMap<APlayerController*, bool> bAttachDoneMap;

	UPROPERTY()
	bool bIsLastItem = false;
	
	// 슬롯 오너 할당 함수
	void SetSlotOwner(int32 SlotIdx, APlayerState* PS);
	void SetSlotAttachedItem(int32 SlotIdx, class AItem* Item);

	UFUNCTION()
	void OnRep_SlotOwners();
	UFUNCTION()
	void OnRep_SlotAttachedItems();

	UPROPERTY(ReplicatedUsing = OnRep_SlotOwners)
	TArray<APlayerState*> SlotOwners;
	UPROPERTY(ReplicatedUsing = OnRep_SlotAttachedItems)
	TArray<class AItem*> SlotAttachedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class USlotComponent*> SlotComponents;
};
