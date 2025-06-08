// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Board.h"
#include "Components/ChildActorComponent.h"
#include "Actor/Shotgun.h"

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
	ShotgunChild->SetRelativeLocation(FVector(-30, 7, 0)); // (X=-30.000000,Y=7.000000,Z=0.000000)
	ShotgunChild->SetRelativeRotation(FRotator(0, 0, -90)); // (Pitch=0.000000,Yaw=0.000000,Roll=-90.000000)
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

