// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SlotComponent.h"
#include "Actor/Item.h"
#include "Components/BoxComponent.h"
#include "Actor/Board.h"
#include "Game/BRGameMode.h"

USlotComponent::USlotComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bHasItem = false;
	AttachedItem = nullptr;
	BoardOwner = nullptr;

}

void USlotComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ClickBox)
	{
		ClickBox = NewObject<UBoxComponent>(this, TEXT("ClickBox"));
		ClickBox->RegisterComponent();
		ClickBox->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ClickBox->SetBoxExtent(FVector(20, 20, 10));
		ClickBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ClickBox->SetCollisionObjectType(ECC_WorldDynamic);
		ClickBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		ClickBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

		ClickBox->OnClicked.AddDynamic(this, &USlotComponent::OnBoxClicked);
	}
}

void USlotComponent::AttachItem(AItem* Item)
{
	if(bHasItem || !Item) return;
	AttachedItem = Item;
	bHasItem = true;

	// 아이템을 슬롯에 Attach
	Item->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Item->SetActorRelativeLocation(FVector::ZeroVector);
	Item->SetActorRelativeRotation(FRotator::ZeroRotator);

	// 아이템에 슬롯 정보를 저장
	Item->SetAttachedSlot(this);
}

void USlotComponent::DetachItem()
{
	if (bHasItem || !AttachedItem) return;

	// 아이템을 슬롯에서 Detach
	AttachedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	AttachedItem->SetAttachedSlot(nullptr);

	AttachedItem = nullptr;
	bHasItem = false;
}

void USlotComponent::OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	// 슬롯이 비어있을 때만 처리
	if(bHasItem || !BoardOwner) return;
	
	// Board에 슬롯이 클릭되었음을 알림
	BoardOwner->OnSlotClicked(this);
	
	// BoardActor나 PlayerController와 상호작용 필요 (현재 선택된 Item 등)
}

