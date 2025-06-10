// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Board.h"
#include "Components/ChildActorComponent.h"
#include "Actor/Shotgun.h"
#include "Actor/Item.h"
#include "Actor/SlotComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ABoard::ABoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Board Mesh
	BoardMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMeshComp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoardPath (TEXT("/Engine/BasicShapes/Cube.Cube"));
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
	static const FVector SlotPositions[8] =
	{
		FVector(-40.f, -80.f, 5.f),
		FVector(-40.f, -40.f, 5.f),
		FVector(-80.f, -80.f, 5.f),
		FVector(-80.f, -40.f, 5.f),
		FVector(-40.f,  40.f, 5.f),
		FVector(-40.f,  80.f, 5.f),
		FVector(-80.f,  40.f, 5.f),
		FVector(-80.f,  80.f, 5.f)
	};

	for (int i = 1; i <= 8; ++i)
	{
		USlotComponent* NewSlot = CreateDefaultSubobject<USlotComponent>(*FString::Printf(TEXT("Slot%d"), i));
		NewSlot->SetupAttachment(RootComponent);
		NewSlot->SetRelativeLocation(SlotPositions[i - 1]);
		NewSlot->SetBoardOwner(this);
		SlotComponents.Add(NewSlot);
	}
}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();

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

void ABoard::OnSlotClicked(USlotComponent* Slot)
{
	if(!Slot || Slot->bHasItem || !PendingItem) return;

	// 슬롯에 아이템을 Attach
	Slot->AttachItem(PendingItem);
	PendingItem = nullptr;

	// 아이템 모두 배치됐는지 체크 -> 박스 제거 등 로직
}

void ABoard::SpawnItem(EItemType ItemType)
{
	// 아이템 박스 클릭 시 호출
    FTransform SpawnTransform = FTransform(FRotator(0), FVector(-60, 0, 20)); // 위치 수정해야함
	AItem* Item = GetWorld()->SpawnActor<AItem>(AItem::StaticClass(), SpawnTransform);
	if (Item)
	{
		PendingItem = Item;
		Item->SetBoardOwner(this); // 아이템 테이블 참조시 필요
	}
}

