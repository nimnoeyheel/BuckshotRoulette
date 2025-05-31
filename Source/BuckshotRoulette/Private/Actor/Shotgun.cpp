// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Shotgun.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMesh.h"

// Sets default values
AShotgun::AShotgun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShotgunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShotgunMesh"));
	RootComponent = ShotgunMesh;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ShotGunMeshAsset(TEXT("/Game/CGrShotgunsPack/Weapons/Meshes/Shotgun_Meshes/SKM_SAShotgun.SKM_SAShotgun"));
	if (ShotGunMeshAsset.Object)
	{
		ShotgunMesh->SetSkeletalMesh(ShotGunMeshAsset.Object);
	}

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetRelativeLocation(FVector(30, 0, 5)); // (X=30.000000,Y=0.000000,Z=5.000000)
	OverlapBox->SetRelativeScale3D(FVector(1.6f, 0.1f, 0.2f)); // (X=1.600000,Y=0.100000,Z=0.200000)
}

// Called when the game starts or when spawned
void AShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShotgun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}