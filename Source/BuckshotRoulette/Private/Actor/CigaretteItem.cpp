// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/CigaretteItem.h"
#include "Components/BoxComponent.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "Game/BRGameMode.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"

ACigaretteItem::ACigaretteItem()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetIsReplicated(false);
	OverlapBox->SetBoxExtent(FVector(30, 40, 25)); // (X=30.000000,Y=40.000000,Z=25.000000)
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	// 이벤트 바인딩
	OverlapBox->OnBeginCursorOver.AddDynamic(this, &ACigaretteItem::OnBeginMouseOver);
	OverlapBox->OnEndCursorOver.AddDynamic(this, &ACigaretteItem::OnEndMouseOver);
	OverlapBox->OnClicked.AddDynamic(this, &ACigaretteItem::OnClicked);

	bReplicates = true;
}

void ACigaretteItem::OnBeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->ShowItemsRuleSubtitle(ItemType);
}

void ACigaretteItem::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void ACigaretteItem::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	ServerRPC_UseItem();
}

void ACigaretteItem::ServerRPC_UseItem_Implementation()
{
	UseItem();
}

void ACigaretteItem::UseItem()
{
	if (!HasAuthority()) return;

	ABRPlayerState* PS = Cast<ABRPlayerState>(OwningPlayer->PlayerState);
	ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
	if(!PS || !GM) return;

	// 최대 HP 넘지 않도록
	int32 MaxHp = GM->AllMatches[GM->CurrentMatchIdx].PlayerHP;
	if(PS->Hp >= MaxHp) return;

	PS->Hp++;
	PS->CigarettesSmoked++;
	PS->TotalCash += (PS->CigarettesSmoked * 100);
	PS->OnRep_Hp();

	Multicast_PlayUseEffect();

	Super::UseItem();
}

void ACigaretteItem::Multicast_PlayUseEffect_Implementation()
{

}
