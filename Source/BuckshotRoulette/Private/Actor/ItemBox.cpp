// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ItemBox.h"
#include "Actor/Board.h"
#include "Components/BoxComponent.h"

// Sets default values
AItemBox::AItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;

	BoxComp->SetBoxExtent(FVector(15));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 아이템박스 외형
	ItemBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemBoxMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemBoxMeshPath (TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (ItemBoxMeshPath.Succeeded())
	{
		ItemBoxMesh->SetStaticMesh(ItemBoxMeshPath.Object);
		ItemBoxMesh->SetupAttachment(RootComponent);
		ItemBoxMesh->SetRelativeScale3D(FVector(0.25));
	}
}

// Called when the game starts or when spawned
void AItemBox::BeginPlay()
{
	Super::BeginPlay();
	
	if(BoxComp) BoxComp->OnClicked.AddDynamic(this, &AItemBox::OnBoxClicked);
}

// Called every frame
void AItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBox::OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	// 아이템이 스폰되고 SlotComp에 Attach되기 전까지 클릭 방지

	if (PendingItems.IsValidIndex(CurrentItemIdx))
	{
		EItemType NextItem = PendingItems[CurrentItemIdx];
		if (BoardOwner) BoardOwner->SpawnItem(NextItem);
		++CurrentItemIdx;

		// 마지막 아이템까지 모두 꺼내면 박스 제거
		if(CurrentItemIdx >= PendingItems.Num()) Destroy();
	}
}

void AItemBox::InitPendingItems(const TArray<EItemType>& Items)
{
	PendingItems = Items;
	CurrentItemIdx = 0;
}

