// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SlotComponent.generated.h"


UCLASS( /*ClassGroup=(Custom), meta=(BlueprintSpawnableComponent)*/ )
class BUCKSHOTROULETTE_API USlotComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USlotComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void AttachItem(class AItem* Item);
	void DetachItem();

    UFUNCTION()
    void OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

public:
	// 보드
	void SetBoardOwner(class ABoard* InBoard) { BoardOwner = InBoard; }
	UPROPERTY()
	class ABoard* BoardOwner;

	// PC
	void SetSlotOwner(APlayerState* PC) { SlotOwner = PC; }
	APlayerState* GetSlotOwner() const { return SlotOwner; }

	UPROPERTY()
	APlayerState* SlotOwner = nullptr;

	// 슬롯에 놓인 아이템 참조
	UPROPERTY()
	class AItem* AttachedItem = nullptr;

	// 슬롯 상태
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool bHasItem = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* ClickBox;
};
