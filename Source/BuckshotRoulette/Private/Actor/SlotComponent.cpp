// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SlotComponent.h"
#include "Components/BoxComponent.h"
#include "Actor/Item.h"
#include "Actor/Board.h"
#include "Game/BRGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BRPlayerController.h"

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

	ClickBox = NewObject<UBoxComponent>(this, TEXT("ClickBox"));
	if (ClickBox)
	{
		ClickBox->RegisterComponent();
		ClickBox->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ClickBox->SetBoxExtent(FVector(18, 18, 10));
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
	bHasItem = true;

	// 서버에서만 Board에 상태 반영 요청
	if (GetOwnerRole() == ROLE_Authority && BoardOwner)
	{
		int32 SlotIdx = BoardOwner->SlotComponents.Find(this);
		if (SlotIdx != INDEX_NONE)
		{
			BoardOwner->SetSlotAttachedItem(SlotIdx, Item);
		}
	}

	// 실제 컴포넌트에도 캐싱(서버에서 바로, 클라에선 OnRep으로 덮어씀)
	AttachedItem = Item;

	// 아이템도 SlotComponent를 참조하도록
	Item->SetAttachedSlot(this);

	// 액터에 붙이기(비주얼)
	Item->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Item->SetActorRelativeLocation(FVector::ZeroVector);
	Item->SetActorRelativeRotation(FRotator::ZeroRotator);
}

void USlotComponent::DetachItem()
{
	if (bHasItem || !AttachedItem) return;

	// 아이템을 슬롯에서 Detach
	AttachedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	AttachedItem->SetAttachedSlot(nullptr);

	// 서버에서만 Board에 상태 반영
	if (GetOwnerRole() == ROLE_Authority && BoardOwner)
	{
		int32 SlotIdx = BoardOwner->SlotComponents.Find(this);
		if (SlotIdx != INDEX_NONE)
		{
			BoardOwner->SetSlotAttachedItem(SlotIdx, nullptr);
		}
	}

	AttachedItem = nullptr;
	bHasItem = false;
}

void USlotComponent::OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Log, TEXT("Clicked On Slot"));

	if (bHasItem || !BoardOwner) return;

	int32 SlotIdx = BoardOwner->SlotComponents.Find(this);
	if (SlotIdx == INDEX_NONE) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC && !PC->HasAuthority())
	{
		// 클라에서 → 본인 PC 통해 서버 호출
		ABRPlayerController* MyPC = Cast<ABRPlayerController>(PC);
		if (MyPC)
		{
			MyPC->ServerRPC_ClickSlot(this, SlotIdx);
		}
		return;
	}

	// 서버에서 직접 처리
	BoardOwner->OnSlotClicked(this, SlotIdx, PC);
}

