// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Types/ItemType.h"
#include "CigaretteItem.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API ACigaretteItem : public AItem
{
	GENERATED_BODY()
	
public:
	ACigaretteItem();

	UFUNCTION()
    void OnBeginMouseOver(UPrimitiveComponent* TouchedComponent);
    UFUNCTION()
    void OnEndMouseOver(UPrimitiveComponent* TouchedComponent);
    UFUNCTION()
    void OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UseItem();

	virtual void UseItem() override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayUseEffect();

protected:
	UPROPERTY()
	EItemType ItemType = EItemType::Cigarette; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//class USkeletalMeshComponent* CigaretteMesh;
	class UStaticMeshComponent* CigaretteMesh;

	// 오버랩 영역 (상호작용)
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* OverlapBox;

};
