﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MagnifierItem.h"
#include "Components/BoxComponent.h"
#include "Player/BRPlayerController.h"
#include "Game/BRGameState.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"

AMagnifierItem::AMagnifierItem()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetIsReplicated(false);
	OverlapBox->SetBoxExtent(FVector(55, 30, 25)); // (X=55.000000,Y=30.000000,Z=25.000000)
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	// 이벤트 바인딩
	OverlapBox->OnBeginCursorOver.AddDynamic(this, &AMagnifierItem::OnBeginMouseOver);
	OverlapBox->OnEndCursorOver.AddDynamic(this, &AMagnifierItem::OnEndMouseOver);
	OverlapBox->OnClicked.AddDynamic(this, &AMagnifierItem::OnClicked);

	bReplicates = true;
}

void AMagnifierItem::OnBeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->ShowItemsRuleSubtitle(ItemType);
}

void AMagnifierItem::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void AMagnifierItem::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	ServerRPC_UseItem();
}

void AMagnifierItem::ServerRPC_UseItem_Implementation()
{
	UseItem();
}

void AMagnifierItem::UseItem()
{
	if (!HasAuthority()) return;

	ABRPlayerController* PC = Cast<ABRPlayerController>(OwningPlayer);
	if (!PC || !PC->IsMyTurn()) return;

	// UI로 현재 총알 타입 확인
	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || GS->AmmoSequence.Num() <= 0) return;

	EAmmoType CurrentAmmo = GS->AmmoSequence[GS->CurrentAmmoIndex];
	PC->ClientRPC_ShowCurrentAmmo(CurrentAmmo);

	Multicast_PlayUseEffect();

	Super::UseItem();
}

void AMagnifierItem::Multicast_PlayUseEffect_Implementation()
{
}
