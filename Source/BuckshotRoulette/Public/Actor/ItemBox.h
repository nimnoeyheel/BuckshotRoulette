// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ItemType.h"
#include "ItemBox.generated.h"

UCLASS()
class BUCKSHOTROULETTE_API AItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

public:
    UFUNCTION()
    void OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION(Server, Reliable)
	void Server_OnBoxClicked();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AItem> ItemClass;

// 보드 참조
	void SetBoardOwner(class ABoard* InBoard) { BoardOwner = InBoard; }
	class ABoard* GetBoardOwner() const { return BoardOwner; }

	UPROPERTY(Replicated)
	class ABoard* BoardOwner;

// 플레이어 소유권
	void SetOwningPlayer(class APlayerController* PC) { OwningPlayer = PC; }
	class APlayerController* GetOwningPlayer() const { return OwningPlayer; }

	UPROPERTY(Replicated)
	class APlayerController* OwningPlayer = nullptr;

// 아이템 관리
	void InitPendingItems(const TArray<EItemType>& Items);

	UPROPERTY(Replicated)
	TArray<EItemType> PendingItems;
	
	UPROPERTY(Replicated)
	int32 CurrentItemIdx = 0;

	bool bIsLastItem = false;

	UPROPERTY(Replicated)
	bool bIsSpawningItem = false;

// 아이템 박스 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* ItemBoxMesh;
};
