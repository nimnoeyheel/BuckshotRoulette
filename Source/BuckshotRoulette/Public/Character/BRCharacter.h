// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BRCharacter.generated.h"

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

public:
// 애니메이션
	UFUNCTION(BlueprintCallable)
	void TriggerAttackAnim();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsAttacking = false;

// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComp;

};
