// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/KnifeItem.h"
#include "Components/BoxComponent.h"
#include "Player/BRPlayerController.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"

AKnifeItem::AKnifeItem()
{
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	RootComponent = OverlapBox;

	OverlapBox->SetBoxExtent(FVector(40, 32, 25)); // (X=40.000000,Y=32.000000,Z=25.000000)
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	// 이벤트 바인딩
	OverlapBox->OnBeginCursorOver.AddDynamic(this, &AKnifeItem::OnBeginMouseOver);
	OverlapBox->OnEndCursorOver.AddDynamic(this, &AKnifeItem::OnEndMouseOver);
	OverlapBox->OnClicked.AddDynamic(this, &AKnifeItem::OnClicked);

	bReplicates = true;
}

void AKnifeItem::OnBeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크

	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->ShowItemsRuleSubtitle(ItemType);
}

void AKnifeItem::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void AKnifeItem::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크

	ServerRPC_UseItem();
}

void AKnifeItem::ServerRPC_UseItem_Implementation()
{
	UseItem();
}

void AKnifeItem::UseItem()
{
	if (!HasAuthority()) return;

	// 데미지 2배

	Multicast_PlayUseEffect();

	Super::UseItem();
}

void AKnifeItem::Multicast_PlayUseEffect_Implementation()
{
}
