// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Item.h"
#include "Actor/SlotComponent.h"
#include "Actor/Board.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// PlaceHolder Mesh
	PlaceHolderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceHolderMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshPath(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MPath(TEXT("/Engine/EditorMaterials/Utilities/LinearColorPicker_MATInst.LinearColorPicker_MATInst"));
	if (MeshPath.Object)
	{
		PlaceHolderMesh->SetStaticMesh(MeshPath.Object);
		PlaceHolderMesh->SetMaterial(0, MPath.Object);
		PlaceHolderMesh->SetupAttachment(RootComponent);
		PlaceHolderMesh->SetRelativeScale3D(FVector(0.2f)); // (X=0.200000,Y=0.200000,Z=0.200000)
	}
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::UseItem()
{
	// 실제 아이템 기능은 파생 클래스에서 override
	// 사용 후 아이템 액터 제거 및 슬롯 Detach
	if(AttachedSlot) AttachedSlot->DetachItem();
	Destroy();
}


