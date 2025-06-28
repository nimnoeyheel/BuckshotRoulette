// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/HandcuffItem.h"
#include "Components/BoxComponent.h"
#include "Player/BRPlayerController.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"
#include "Player/BRPlayerState.h"

AHandcuffItem::AHandcuffItem()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetIsReplicated(false);
	OverlapBox->SetBoxExtent(FVector(55, 40, 20)); // (X=55.000000,Y=40.000000,Z=20.000000)
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	// 이벤트 바인딩
	OverlapBox->OnBeginCursorOver.AddDynamic(this, &AHandcuffItem::OnBeginMouseOver);
	OverlapBox->OnEndCursorOver.AddDynamic(this, &AHandcuffItem::OnEndMouseOver);
	OverlapBox->OnClicked.AddDynamic(this, &AHandcuffItem::OnClicked);

	bReplicates = true;
}

void AHandcuffItem::OnBeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->ShowItemsRuleSubtitle(ItemType);
}

void AHandcuffItem::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	OverlapBox->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void AHandcuffItem::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	ServerRPC_UseItem();
}

void AHandcuffItem::ServerRPC_UseItem_Implementation()
{
	UseItem();
}

void AHandcuffItem::UseItem()
{
	if (!HasAuthority()) return;

	ABRPlayerState* PS = Cast<ABRPlayerState>(OwningPlayer ? OwningPlayer->PlayerState : nullptr);
	if (PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HANDCUFF] %s skip opponent turn"), *PS->GetPlayerName());
		PS->SetSkipOpponentTurn(true);
	}

	Multicast_PlayUseEffect();

	Super::UseItem();
}

void AHandcuffItem::Multicast_PlayUseEffect_Implementation()
{
}
