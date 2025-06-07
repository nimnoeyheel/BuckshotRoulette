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
// 애니메이션
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerAttackAnim();

	UFUNCTION()
	void TriggerAttackAnim();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsAttacking = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsDamaged = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsDead = false;

// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;

};
