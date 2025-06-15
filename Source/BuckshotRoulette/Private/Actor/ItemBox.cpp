// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/ItemBox.h"
#include "Actor/Board.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BRGameMode.h"
#include "Player/BRPlayerController.h"
#include "Net/UnrealNetwork.h"

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
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MPath(TEXT("/Engine/VREditor/Teleport/TeleportDirectionMaterial.TeleportDirectionMaterial"));
	if (ItemBoxMeshPath.Succeeded() && MPath.Succeeded())
	{
		ItemBoxMesh->SetStaticMesh(ItemBoxMeshPath.Object);
		ItemBoxMesh->SetMaterial(0, MPath.Object);
		ItemBoxMesh->SetupAttachment(RootComponent);
		ItemBoxMesh->SetRelativeScale3D(FVector(0.25));
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void AItemBox::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicates(true);

	if(BoxComp) BoxComp->OnClicked.AddDynamic(this, &AItemBox::OnBoxClicked);
}

void AItemBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItemBox, BoardOwner);
	DOREPLIFETIME(AItemBox, OwningPlayer);
	DOREPLIFETIME(AItemBox, PendingItems);
	DOREPLIFETIME(AItemBox, CurrentItemIdx);
}

// Called every frame
void AItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBox::Destroyed()
{
	Super::Destroyed();

	UE_LOG(LogTemp, Log, TEXT("AItemBox::Destroyed()"));

	// GameMode에 박스 소멸 알림
	if (HasAuthority())
	{
		ABRGameMode* GM = Cast<ABRGameMode>(GetWorld()->GetAuthGameMode());
		if(GM) GM->NotifyItemBoxDestroyed();
	}
}

void AItemBox::OnBoxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Log, TEXT("AItemBox::OnBoxClicked"));

	// 플레이어 권한 체크
	APlayerController* MyPC = UGameplayStatics::GetPlayerController(this, 0);
	if (MyPC != OwningPlayer) return;

	Server_OnBoxClicked();
}

void AItemBox::Server_OnBoxClicked_Implementation()
{
	// 아이템이 스폰되고 SlotComp에 Attach되기 전까지 클릭 방지
	if (PendingItems.IsValidIndex(CurrentItemIdx))
	{
		EItemType NextItem = PendingItems[CurrentItemIdx];
		if (BoardOwner)
		{
			++CurrentItemIdx;
			bIsLastItem = CurrentItemIdx >= PendingItems.Num() ? true : false;
			BoardOwner->SpawnItem(NextItem, OwningPlayer, bIsLastItem);
		}
	}
}

void AItemBox::InitPendingItems(const TArray<EItemType>& Items)
{
	PendingItems = Items;
	CurrentItemIdx = 0;
}

