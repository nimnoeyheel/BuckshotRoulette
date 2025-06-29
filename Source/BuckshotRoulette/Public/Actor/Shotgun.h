// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shotgun.generated.h"

UCLASS()
class BUCKSHOTROULETTE_API AShotgun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShotgun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ShotgunMesh;

	// 오버랩 영역 (상호작용)
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* OverlapBox;

    UFUNCTION()
    void OnBeginMouseOver(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void OnEndMouseOver(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void OnClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

// 애니메이션 실행 시 호버 기능 방지
	UPROPERTY(Replicated)
	bool bIsInteractive = true;

	UPROPERTY()
	class ABRCharacter* OwningCharacter = nullptr;

public:
	void SetOwningCharacter(ABRCharacter* Char) { OwningCharacter = Char; }

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetInteractionEnabled(bool bEnabled);

public:
// Self Fire
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TriggerSelfFireAnim(bool bIsServer, bool bIsLiveAmmo);

	UFUNCTION()
	void SetAnimBPsFiringValue(const FString VarName);

// Mongtage
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* Montage_SelfFire_Server_Live;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* Montage_SelfFire_Client_Live;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* Montage_SelfFire_Server_Blank;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* Montage_SelfFire_Client_Blank;
};