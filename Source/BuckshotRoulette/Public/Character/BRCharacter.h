// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BRCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerAnimState : uint8
{
	Idle      UMETA(DisplayName = "Idle"),
	Attack    UMETA(DisplayName = "Attack"),
	Damage    UMETA(DisplayName = "Damage"),
	Death     UMETA(DisplayName = "Death")
};

UCLASS()
class BUCKSHOTROULETTE_API ABRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
// 총 Attach
	class AShotgun* GetShotgunActor() const;

	void AttachShotgunToHand();
	void AttachShotgunToBoard();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board")
	class ABoard* BoardActor = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shotgun")
	class AShotgun* ShotgunActor = nullptr;

// 애니메이션
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerAttackAnim(bool bIsLiveAmmo);
	UFUNCTION()
	void TriggerAttackAnim(bool bIsLiveAmmo);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerDamageAnim();
	UFUNCTION()
	void TriggerDamageAnim();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerDeathAnim();
	UFUNCTION()
	void TriggerDeathAnim();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category="Anim")
	EPlayerAnimState PlayerAnimState = EPlayerAnimState::Idle;

// 몽타주
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	class UAnimMontage* Montage_Attack_Live;
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	class UAnimMontage* Montage_Attack_Blank;

// 소유한 아이템
	void GetOwnedItems(TArray<class AItem*>& OutItems) const;

// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;

};
