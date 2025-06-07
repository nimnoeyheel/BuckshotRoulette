// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BRCharacter.h"
#include "Player/BRPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABRCharacter::ABRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	CameraComp->SetRelativeLocation(FVector(30, 0, 40)); // (X=30.000000,Y=0.000000,Z=40.000000)
	CameraComp->SetRelativeRotation(FRotator(-10, 0, 0)); // (Pitch=-10.000000,Yaw=0.000000,Roll=0.000000)
}

// Called when the game starts or when spawned
void ABRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ABRPlayerState* PS = GetPlayerState<ABRPlayerState>())
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerName: %s"), *PS->GetPlayerName());
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

void ABRCharacter::Multicast_TriggerAttackAnim_Implementation()
{
	TriggerAttackAnim();
}

void ABRCharacter::TriggerAttackAnim()
{
	bIsAttacking = true;
	PlayerAnimState = EPlayerAnimState::Attack;

	// 2.57초 뒤 (FireAnim Sec)
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,
		FTimerDelegate::CreateLambda([&]()
		{
			bIsAttacking = false;
			PlayerAnimState = EPlayerAnimState::Idle;
		}
	), 2.57f, false);
}

void ABRCharacter::Multicast_TriggerDamageAnim_Implementation()
{
	TriggerDamageAnim();
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
		}
	), 2.55f, false);
}

void ABRCharacter::Multicast_TriggerDeathAnim_Implementation()
{
	TriggerDeathAnim();
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
		}
	), 1.96f, false);
}
