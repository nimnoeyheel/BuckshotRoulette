﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Types/ItemType.h"
#include "MagnifierItem.generated.h"

/**
 * 
 */
UCLASS()
class BUCKSHOTROULETTE_API AMagnifierItem : public AItem
{
	GENERATED_BODY()
	
public:
	AMagnifierItem();

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
	EItemType ItemType = EItemType::Magnifier; 

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootScene;

	// 오버랩 영역 (상호작용)
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* OverlapBox;
};
