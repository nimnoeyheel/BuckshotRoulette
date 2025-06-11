// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class BUCKSHOTROULETTE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 아이템 사용
	virtual void UseItem();

	void SetAttachedSlot(class USlotComponent* Slot) { AttachedSlot = Slot; }
	class USlotComponent* GetAttachedSlot() const { return AttachedSlot; }

	UPROPERTY()
	class USlotComponent* AttachedSlot;

	void SetBoardOwner(class ABoard* InBoard) { BoardOwner = InBoard; }
	class ABoard* GetBoardOwner() const {return BoardOwner; }

	UPROPERTY()
	class ABoard* BoardOwner = nullptr;

// PlaceHolderMesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlaceHolderMesh;
};
