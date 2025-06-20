// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BeerItem.h"
#include "Components/BoxComponent.h"
#include "Player/BRPlayerController.h"
#include "Player/BRPlayerState.h"
#include "Game/BRGameState.h"
#include "UI/MainWidget.h"
#include "UI/InGameWidget.h"
#include "Actor/SlotComponent.h"
#include "Game/BRGameMode.h"

ABeerItem::ABeerItem()
{
	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	RootComponent = OverlapBox;

	OverlapBox->SetBoxExtent(FVector(2.5, 2.5, 10)); // (X=2.500000,Y=2.500000,Z=10.000000)
	OverlapBox->SetRelativeScale3D(FVector(7));
	OverlapBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapBox->SetCollisionObjectType(ECC_WorldDynamic);
	OverlapBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	OverlapBox->SetGenerateOverlapEvents(true);

	BeerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeerMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BeerMeshAsset(TEXT("/Game/BuckShotRoulette/Blueprints/Items/Mesh/SM_Redbull.SM_Redbull"));
	if (BeerMeshAsset.Object)
	{
		BeerMesh->SetStaticMesh(BeerMeshAsset.Object);
		BeerMesh->SetupAttachment(RootComponent);
		BeerMesh->SetRelativeLocation(FVector(0, 3.5, -1.5)); // (X=0.000000,Y=3.500000,Z=-1.500000)
		BeerMesh->SetIsReplicated(false);
	}

	/*static ConstructorHelpers::FClassFinder<UAnimInstance> AnimPath(TEXT(""));
	if (AnimPath.Class)
	{
		BeerMesh->SetAnimInstanceClass(AnimPath.Class);
	}*/

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
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, 5);
	OverlapBox->SetWorldLocation(Loc);
	
	PC->MainUI->InGameUI->ShowItemsRuleSubtitle(ItemType);
}

void ABeerItem::OnEndMouseOver(UPrimitiveComponent* TouchedComponent)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->MainUI || !PC->MainUI->InGameUI || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

	FVector Loc = GetActorLocation() + FVector(0, 0, -5);
	OverlapBox->SetWorldLocation(Loc);
	
	PC->MainUI->InGameUI->SetVisibleSubtitle(false);
}

void ABeerItem::OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	ABRPlayerController* PC = Cast<ABRPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC || !PC->IsMyTurn()) return;
	if (!IsOwnedByLocalPlayer()) return; // 슬롯 소유자 체크
	if (!bIsInteractive) return;

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
	
	// 발사 시 탄 하나 건너뛰도록 설정
	if (GS->AmmoSequence.IsValidIndex(GS->CurrentAmmoIndex))
	{
		GS->CurrentAmmoIndex++;
	}

	// 사용자의 PlayerState 참조해서 사용 수치 갱신
	if (ABRPlayerState* PS = Cast<ABRPlayerState>(OwningPlayer ? OwningPlayer->PlayerState : nullptr))
	{
		PS->MLOfBeerDrank++;
		PS->TotalCash += (PS->MLOfBeerDrank * 250);
	}

	// 애니메이션/이펙트 알림
	Multicast_PlayUseEffect();

	// 버린 총알이 마지막 총알이였다면 즉시 라운드 종료
	if (GS->CurrentAmmoIndex == GS->AmmoSequence.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("Beer Removed last ammo. Ending round."));

		ABRGameMode* GM = GetWorld()->GetAuthGameMode<ABRGameMode>();
		GM->OnRoundEnd();
	}

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
