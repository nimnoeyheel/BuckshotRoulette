// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Shotgun.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UI/MainWidget.h"
#include "Player/BRPlayerController.h"
#include "UI/InGameWidget.h"

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

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimPath(TEXT("/Game/BuckShotRoulette/Blueprints/Shotgun/ABP_Shotgun.ABP_Shotgun_C"));
	if (AnimPath.Class)
	{
		ShotgunMesh->SetAnimInstanceClass(AnimPath.Class);
	}

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetRelativeLocation(FVector(30, 0, 5)); // (X=30.000000,Y=0.000000,Z=5.000000)
	OverlapBox->SetRelativeScale3D(FVector(1.6f, 0.35f, 0.45f)); // (X=1.600000,Y=0.350000,Z=0.450000)
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	// 이벤트 바인딩
	OverlapBox->OnBeginCursorOver.AddDynamic(this, &AShotgun::OnBeginMouseOver);
	OverlapBox->OnEndCursorOver.AddDynamic(this, &AShotgun::OnEndMouseOver);
	OverlapBox->OnClicked.AddDynamic(this, &AShotgun::OnClicked);
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

void AShotgun::OnBeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	
	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	ShotgunMesh->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->ShowFireRuleSubtitle();
}

void AShotgun::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	ShotgunMesh->SetWorldLocation(Loc);

	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void AShotgun::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	// 타겟 선택 UI 열기 (자신/상대)
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;

	PC->MainUI->InGameUI->ShowTargetSelectUI();
}

void AShotgun::Multicast_TriggerSelfFireAnim_Implementation(bool bIsServer)
{
	if (bIsServer)
	{
		SetAnimBPsFiringValue(TEXT("bIsServerFiring"));
	}
	else
	{
		SetAnimBPsFiringValue(TEXT("bIsClientFiring"));
	}
}

void AShotgun::SetAnimBPsFiringValue(const FString VarName)
{
	if (ShotgunMesh && ShotgunMesh->GetAnimInstance())
	{
		UAnimInstance* AnimBP = ShotgunMesh->GetAnimInstance();
		FName VarName_Firing(VarName);

		// 변수 존재 체크 후 Set
		if(FBoolProperty* Prop = FindFProperty<FBoolProperty>(AnimBP->GetClass(), VarName_Firing))
		{
			Prop->SetPropertyValue_InContainer(AnimBP, true);

			// (SelfFireAnim Sec)
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle,
				[AnimBP, VarName_Firing]()
				{
					if (FBoolProperty* PropInner = FindFProperty<FBoolProperty>(AnimBP->GetClass(), VarName_Firing))
					{
						PropInner->SetPropertyValue_InContainer(AnimBP, false);
					}
				},
			2.23f, false);
		}
	}
}