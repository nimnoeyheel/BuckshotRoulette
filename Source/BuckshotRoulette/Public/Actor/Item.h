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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 아이템 사용
	virtual void UseItem();

public:
	void SetAttachedSlot(class USlotComponent* Slot) { AttachedSlot = Slot; }
	class USlotComponent* GetAttachedSlot() const { return AttachedSlot; }

	UPROPERTY()
	class USlotComponent* AttachedSlot;

	void SetBoardOwner(class ABoard* InBoard) { BoardOwner = InBoard; }
	class ABoard* GetBoardOwner() const {return BoardOwner; }

	UFUNCTION()
	void OnRep_BoardOwner();

	UPROPERTY(ReplicatedUsing = OnRep_BoardOwner)
	class ABoard* BoardOwner = nullptr;

// 플레이어 소유권
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool IsOwnedByLocalPlayer() const;

	void SetOwningPlayer(class APlayerController* PC) { OwningPlayer = PC; }
	class APlayerController* GetOwningPlayer() const { return OwningPlayer; }

	UPROPERTY(Replicated)
	class APlayerController* OwningPlayer = nullptr;

// PlaceHolderMesh
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//UStaticMeshComponent* PlaceHolderMesh;
};
