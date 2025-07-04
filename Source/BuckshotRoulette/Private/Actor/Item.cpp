﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Item.h"
#include "Actor/SlotComponent.h"
#include "Actor/Board.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Player/BRPlayerController.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// PlaceHolder Mesh
	//PlaceHolderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceHolderMesh"));
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshPath(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MPath(TEXT("/Engine/EditorMaterials/Utilities/LinearColorPicker_MATInst.LinearColorPicker_MATInst"));
	//if (MeshPath.Object)
	//{
	//	PlaceHolderMesh->SetStaticMesh(MeshPath.Object);
	//	PlaceHolderMesh->SetMaterial(0, MPath.Object);
	//	PlaceHolderMesh->SetupAttachment(RootComponent);
	//	PlaceHolderMesh->SetRelativeScale3D(FVector(0.2f)); // (X=0.200000,Y=0.200000,Z=0.200000)
	//}

	bReplicates = true;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicates(true);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
   DOREPLIFETIME(AItem, OwningPlayer);
   DOREPLIFETIME(AItem, BoardOwner);
   DOREPLIFETIME(AItem, bIsInteractive);
}

void AItem::UseItem()
{
	// 실제 아이템 기능은 파생 클래스에서 override
	// 사용 후 아이템 액터 제거 및 슬롯 Detach
	if(AttachedSlot) AttachedSlot->DetachItem();
	Destroy();
}

void AItem::OnRep_BoardOwner()
{
}

bool AItem::IsOwnedByLocalPlayer() const
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !BoardOwner) return false;

	// 슬롯 인덱스 추출
	int32 SlotIdx = BoardOwner->GetSlotAttachedItems().Find(const_cast<AItem*>(this));
	if (!BoardOwner->GetSlotOwners().IsValidIndex(SlotIdx)) return false;

	// 슬롯 소유자가 내 PlayerState와 같은지 비교
	return BoardOwner->GetSlotOwners()[SlotIdx] == PC->PlayerState;
}

void AItem::Multicast_SetItemsInteractionEnabled_Implementation(bool bEnabled)
{
	bIsInteractive = bEnabled;
}