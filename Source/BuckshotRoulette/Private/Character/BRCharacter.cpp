// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRCharacter.h"
#include "Player/BRPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Board.h"
#include "Actor/Shotgun.h"
#include "EngineUtils.h"
#include "Actor/Item.h"

// Sets default values
ABRCharacter::ABRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent->SetRelativeScale3D(FVector(3));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshPath(TEXT("/Game/MANIACS/ManiacKiller/Mesh/Maniac4/SK_maniac_killer_4.SK_maniac_killer_4"));
	if (MeshPath.Object)
	{
		GetMesh()->SetSkeletalMesh(MeshPath.Object);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -90)); //(X=0.000000,Y=0.000000,Z=-90.000000)
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0)); //(Pitch=0.000000,Yaw=-90.000000,Roll=0.000000)
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimPath(TEXT("/Game/BuckShotRoulette/Blueprints/Character/ABP_Player.ABP_Player_C"));
	if (AnimPath.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimPath.Class);
	}

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->SetRelativeLocation(FVector(30, 0, 58)); // (X=30.000000,Y=0.000000,Z=58.000000)
	CameraComp->SetRelativeRotation(FRotator(-26, 0, 0)); // (Pitch=-26.000000,Yaw=0.000000,Roll=0.000000)

	// 몽타주
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageLiveObj(TEXT("/Game/BuckShotRoulette/Blueprints/Character/AM_Fire_Live.AM_Fire_Live"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageBlankObj(TEXT("/Game/BuckShotRoulette/Blueprints/Character/AM_Fire_Blank.AM_Fire_Blank"));
	if (MontageLiveObj.Succeeded() && MontageBlankObj.Succeeded())
	{
		Montage_Attack_Live = MontageLiveObj.Object;
		Montage_Attack_Blank = MontageBlankObj.Object;
	}
}

// Called when the game starts or when spawned
void ABRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ABRPlayerState* PS = GetPlayerState<ABRPlayerState>())
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerName: %s"), *PS->GetPlayerName());
	}

	// 월드에 존재하는 ABoard 인스턴스를 찾기
	for (TActorIterator<ABoard> It(GetWorld()); It; ++It)
	{
		BoardActor = *It;
		break;
	}

	// 월드에 존재하는 AShotgun 인스턴스를 찾기
	for (TActorIterator<AShotgun> It(GetWorld()); It; ++It)
	{
		ShotgunActor = *It;
		break;
	}
}

// Called every frame
void ABRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABRCharacter, PlayerAnimState);
}

AShotgun* ABRCharacter::GetShotgunActor() const
{
	return ShotgunActor ? ShotgunActor : nullptr;
}

void ABRCharacter::AttachShotgunToHand()
{
	if (BoardActor && BoardActor->GetShotgunActor())
	{
		BoardActor->GetShotgunActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		BoardActor->GetShotgunActor()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "shotgun_Socket");
	}
}

void ABRCharacter::AttachShotgunToBoard()
{
	if (BoardActor && BoardActor->GetShotgunActor())
	{
		BoardActor->GetShotgunActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		BoardActor->GetShotgunActor()->AttachToComponent(BoardActor->ShotgunChild, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ABRCharacter::Multicast_TriggerAttackAnim_Implementation(bool bIsLiveAmmo)
{
	AttachShotgunToHand();
	TriggerAttackAnim(bIsLiveAmmo);
}

void ABRCharacter::TriggerAttackAnim(bool bIsLiveAmmo)
{
	PlayerAnimState = EPlayerAnimState::Attack;

	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		UAnimMontage* AttackMontage = bIsLiveAmmo ? Montage_Attack_Live : Montage_Attack_Blank;
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}

	// 2.5초 뒤 (FireAnim Sec)
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			PlayerAnimState = EPlayerAnimState::Idle;
			AttachShotgunToBoard();

			// 마우스 호버 허용
			if (ShotgunActor) ShotgunActor->Multicast_SetInteractionEnabled(true);

			TArray<AItem*> MyItems;
			GetOwnedItems(MyItems);
			for (AItem* Item : MyItems)
			{
				if (Item) Item->Multicast_SetItemsInteractionEnabled(true);
			}
		}
	), 2.5f, false);
}

void ABRCharacter::Multicast_TriggerDamageAnim_Implementation()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			TriggerDamageAnim();
		}
	), 2.5f, false);
}

void ABRCharacter::TriggerDamageAnim()
{
	PlayerAnimState = EPlayerAnimState::Damage;

	// (DamageAnim Sec)
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			PlayerAnimState = EPlayerAnimState::Idle;

			// 마우스 호버 허용
			if (ShotgunActor) ShotgunActor->Multicast_SetInteractionEnabled(true);

			TArray<AItem*> MyItems;
			GetOwnedItems(MyItems);
			for (AItem* Item : MyItems)
			{
				if (Item) Item->Multicast_SetItemsInteractionEnabled(true);
			}
		}
	), 3.f, false);
}

void ABRCharacter::Multicast_TriggerDeathAnim_Implementation()
{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			TriggerDeathAnim();
		}
	), 2.55f, false);
}

void ABRCharacter::TriggerDeathAnim()
{
	PlayerAnimState = EPlayerAnimState::Death;

	// 1.96초 뒤 (DeathAnim Sec)
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			PlayerAnimState = EPlayerAnimState::Idle;

			// 마우스 호버 허용
			if (ShotgunActor) ShotgunActor->Multicast_SetInteractionEnabled(true);

			TArray<AItem*> MyItems;
			GetOwnedItems(MyItems);
			for (AItem* Item : MyItems)
			{
				if (Item) Item->Multicast_SetItemsInteractionEnabled(true);
			}
		}
	), 1.96f, false);
}

void ABRCharacter::GetOwnedItems(TArray<class AItem*>& OutItems) const
{
	OutItems.Empty();
	if (!BoardActor) return;

	APlayerState* MyPS = GetPlayerState<APlayerState>();
	if (!MyPS) return;

	const TArray<AItem*>& AllItems = BoardActor->GetSlotAttachedItems();
	const TArray<APlayerState*>& AllOwners = BoardActor->GetSlotOwners();

	for (int32 i = 0; i < AllItems.Num(); ++i)
	{
		if (AllOwners.IsValidIndex(i) && AllItems.IsValidIndex(i))
		{
			if (AllOwners[i] == MyPS && AllItems[i] != nullptr)
			{
				OutItems.Add(AllItems[i]);
			}
		}
	}
}
