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
	void OnSlotClicked(class USlotComponent* Slot);

	// 아이템 스폰
	void SpawnItem(EItemType ItemType);

	// 현재 선택된(박스에서 꺼낸) 아이템 추적
	UPROPERTY()
	class AItem* PendingItem = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<class USlotComponent*> SlotComponents;
};
