// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Board.h"
#include "Components/ChildActorComponent.h"
#include "Actor/Shotgun.h"
#include "Actor/Item.h"
#include "Actor/SlotComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Actor/ItemBox.h"

// Sets default values
ABoard::ABoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Board Mesh
	BoardMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMeshComp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoardPath(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (BoardPath.Object)
	{
		BoardMeshComp->SetStaticMesh(BoardPath.Object);
		BoardMeshComp->SetupAttachment(RootComponent);
		BoardMeshComp->SetRelativeScale3D(FVector(2, 2, 0.1f)); // (X=2.000000,Y=2.000000,Z=0.100000)
	}

	// ShotGun
	GunAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GunAttachPoint"));
	GunAttachPoint->SetupAttachment(RootComponent);
	GunAttachPoint->SetRelativeLocation(FVector(0, 0, 10)); // (X=0.000000,Y=0.000000,Z=10.000000)

	ShotgunChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("ShotgunChild"));
	ShotgunChild->SetChildActorClass(AShotgun::StaticClass());
	ShotgunChild->SetupAttachment(GunAttachPoint);
	ShotgunChild->SetRelativeLocation(FVector(-30, 0, 0)); // (X=-30.000000,Y=7.000000,Z=0.000000)
	ShotgunChild->SetRelativeRotation(FRotator(0, 0, -90)); // (Pitch=0.000000,Yaw=0.000000,Roll=-90.000000)

	// Slot
	static const FVector SlotPositions[16] =
	{
		// 서버 슬롯
		FVector(80.f, -80.f, 10.f),
		FVector(80.f, -40.f, 10.f),
		FVector(40.f, -80.f, 10.f),
		FVector(40.f, -40.f, 10.f),
		FVector(80.f,  40.f, 10.f),
		FVector(80.f,  40.f, 10.f),
		FVector(40.f,  40.f, 10.f),
		FVector(40.f,  40.f, 10.f),

		// 클라 슬롯
		FVector(-40.f, -80.f, 10.f),
		FVector(-40.f, -40.f, 10.f),
		FVector(-80.f, -80.f, 10.f),
		FVector(-80.f, -40.f, 10.f),
		FVector(-40.f,  40.f, 10.f),
		FVector(-40.f,  80.f, 10.f),
		FVector(-80.f,  40.f, 10.f),
		FVector(-80.f,  80.f, 10.f)
	};

	for (int i = 0; i < 16; ++i)
	{
		USlotComponent* NewSlot = CreateDefaultSubobject<USlotComponent>(*FString::Printf(TEXT("Slot%d"), i + 1));
		NewSlot->SetupAttachment(RootComponent);
		NewSlot->SetRelativeLocation(SlotPositions[i]);
		SlotComponents.Add(NewSlot);
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

	// Board의 SlotOwner/SlotAttachedItems 초기화
	SlotOwners.SetNum(SlotComponents.Num());
	SlotAttachedItems.SetNum(SlotComponents.Num());

	// 슬롯에 BoardOwner 할당
	for (int i = 0; i < 16; ++i)
	{
		SlotComponents[i]->SetBoardOwner(this);
	}
}

void ABoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABoard, SlotOwners);
	DOREPLIFETIME(ABoard, SlotAttachedItems);
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

AActor* ABoard::GetShotgunActor() const
{
	return ShotgunChild ? ShotgunChild->GetChildActor() : nullptr;
}

#include "EngineUtils.h" // Ensure this include is present for TActorIterator

void ABoard::OnSlotClicked(class USlotComponent* Slot, int32 SlotIdx, APlayerController* RequestingPlayer)
{
   if (!Slot || Slot->bHasItem) return;
   if (!SlotOwners.IsValidIndex(SlotIdx)) return;
   if (!RequestingPlayer || !RequestingPlayer->PlayerState) return;

   // 내 슬롯 소유자인지 체크
   if (SlotOwners[SlotIdx] != RequestingPlayer->PlayerState) return;
   if (!PendingItems.Contains(RequestingPlayer)) return;

   AItem* Item = PendingItems[RequestingPlayer];
   if (!Item) return;

   // 슬롯에 아이템을 Attach
   Slot->AttachItem(Item); // 서버에서 실행하면 Replicated 자동 동기화
   PendingItems.Remove(RequestingPlayer);

   // 아이템 모두 배치됐는지 체크 -> 박스 제거 등 로직
   /*if (bIsLastItem)
   {
	   for (TActorIterator<AItemBox> ItemBox(GetWorld()); ItemBox; ++ItemBox)
	   {
		   AItemBox* Box = Cast<AItemBox>(*ItemBox);
		   if (Box->GetOwningPlayer() == RequestingPlayer)
		   {
			   Box->Destroy();
			   bIsLastItem = false;
		   }
	   }
   }*/
}

void ABoard::SpawnItem(EItemType ItemType, APlayerController* ForPlayer, bool _bIsLastItem)
{
	// 아이템 박스 클릭 시 호출
	// 서버 FVector(420, 580, 115)
	// 클라 FVector(575, 580, 115)
	FVector SpawnLoc;
	if (ForPlayer->HasAuthority()) SpawnLoc = FVector(420, 580, 115);
	else SpawnLoc = FVector(575, 580, 115);

	FTransform SpawnTransform = FTransform(FRotator(0), SpawnLoc);
	AItem* Item = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnTransform);
	if (Item)
	{
		Item->SetReplicates(true);
		Item->SetReplicateMovement(true);
		Item->SetBoardOwner(this);
		Item->SetOwningPlayer(ForPlayer);
		PendingItems.Add(ForPlayer, Item);
		bIsLastItem = _bIsLastItem;
		//PendingItem = Item;
	}
}

void ABoard::SetSlotOwner(int32 SlotIdx, APlayerState* PS)
{
	if (SlotOwners.IsValidIndex(SlotIdx))
	{
		SlotOwners[SlotIdx] = PS;
		// 서버에서는 바로 SlotComponent에도 반영
		if (SlotComponents.IsValidIndex(SlotIdx))
		{
			SlotComponents[SlotIdx]->SlotOwner = PS;
		}
	}
}

void ABoard::SetSlotAttachedItem(int32 SlotIdx, AItem* Item)
{
	if (SlotAttachedItems.IsValidIndex(SlotIdx))
	{
		SlotAttachedItems[SlotIdx] = Item;
		// 서버에서는 바로 SlotComponent에도 반영
		if (SlotComponents.IsValidIndex(SlotIdx))
		{
			SlotComponents[SlotIdx]->AttachedItem = Item;
		}
	}
}

void ABoard::OnRep_SlotOwners()
{
	for (int32 i = 0; i < SlotComponents.Num(); ++i)
	{
		if (SlotComponents.IsValidIndex(i))
		{
			SlotComponents[i]->SlotOwner = SlotOwners.IsValidIndex(i) ? SlotOwners[i] : nullptr;
		}
	}
}

void ABoard::OnRep_SlotAttachedItems()
{
	for (int32 i = 0; i < SlotComponents.Num(); ++i)
	{
		if (SlotComponents.IsValidIndex(i))
		{
			SlotComponents[i]->AttachedItem = SlotAttachedItems.IsValidIndex(i) ? SlotAttachedItems[i] : nullptr;
		}
	}
}

