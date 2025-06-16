// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BeerItem.h"
#include "Components/BoxComponent.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "Game/BRGameState.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"
#include "Actor/SlotComponent.h"

ABeerItem::ABeerItem()
{
	BeerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeerMesh"));
	RootComponent = BeerMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BeerMeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (BeerMeshAsset.Object)
	{
		//BeerMesh->SetSkeletalMesh(BeerMeshAsset.Object);
		BeerMesh->SetStaticMesh(BeerMeshAsset.Object);
		BeerMesh->SetRelativeScale3D(FVector(0.2));
	}

	/*static ConstructorHelpers::FClassFinder<UAnimInstance> AnimPath(TEXT(""));
	if (AnimPath.Class)
	{
		BeerMesh->SetAnimInstanceClass(AnimPath.Class);
	}*/

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(RootComponent);
	OverlapBox->SetRelativeScale3D(FVector(1.5, 1.5, 2)); // (X=1.500000,Y=1.500000,Z=2.000000)
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	// 이벤트 바인딩
	OverlapBox->OnBeginCursorOver.AddDynamic(this, &ABeerItem::OnBeginMouseOver);
	OverlapBox->OnEndCursorOver.AddDynamic(this, &ABeerItem::OnEndMouseOver);
	OverlapBox->OnClicked.AddDynamic(this, &ABeerItem::OnClicked);

	bReplicates = true;
}

void ABeerItem::OnBeginMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크

	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	BeerMesh->SetWorldLocation(Loc);
	
	PC->MainUI->InGameUI->ShowItemsRuleSubtitle(ItemType);
}

void ABeerItem::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	BeerMesh->SetWorldLocation(Loc);
	
	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void ABeerItem::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크

	ServerRPC_UseItem();
}

void ABeerItem::ServerRPC_UseItem_Implementation()
{
	UseItem();
}

void ABeerItem::UseItem()
{
	if (!HasAuthority()) return;

	ABRGameState* GS = GetWorld()->GetGameState<ABRGameState>();
	if (!GS || GS->AmmoSequence.Num() <= 0) return;

	// 탄창에서 현재 총알 하나 제거
	GS->RemoveNextAmmo();

	// 사용자의 PlayerState 참조해서 사용 수치 갱신
	if (OwningPlayer)
	{
		ABRPlayerState* PS = Cast<ABRPlayerState>(OwningPlayer->PlayerState);
		if (PS)
		{
			PS->MLOfBeerDrank++;
		}
	}

	// 애니메이션/이펙트 알림
	Multicast_PlayUseEffect();

	// 슬롯에서 아이템 Detach 및 Destroy
	Super::UseItem();
}

void ABeerItem::Multicast_PlayUseEffect_Implementation()
{
	// 이펙트, 사운드, 애니메이션 실행
	//UGameplayStatics::PlaySoundAtLocation(this, DrinkSound, GetActorLocation());

	// 캐릭터 애니메이션
	/*if (OwningPlayer)
	{
		APawn* Pawn = OwningPlayer->GetPawn();
		if (ABRCharacter* Char = Cast<ABRCharacter>(Pawn))
		{
			Char->PlayDrinkAnim();
		}
	}*/
}
