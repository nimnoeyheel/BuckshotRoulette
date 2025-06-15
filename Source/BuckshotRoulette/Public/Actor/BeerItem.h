// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Types/ItemType.h"
#include "BeerItem.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ABeerItem : public AItem
{
	GENERATED_BODY()
	
public:
	ABeerItem();
	
    UFUNCTION()
    void OnBeginMouseOver(UPrimitiveComponent* TouchedComponent);
    UFUNCTION()
    void OnEndMouseOver(UPrimitiveComponent* TouchedComponent);
    UFUNCTION()
    void OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UseItem();

	virtual void UseItem() override;

public:
	UPROPERTY()
	EItemType ItemType = EItemType::Beer; 

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class USkeletalMeshComponent* BeerMesh;
	class UStaticMeshComponent* BeerMesh;

	// 오버랩 영역 (상호작용)
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* OverlapBox;
};
