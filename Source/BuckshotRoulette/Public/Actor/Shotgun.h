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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* ShotgunMesh;

	// 오버랩 영역 (상호작용)
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* OverlapBox;
};