// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Live UMETA(DisplayName = "Live"),
	Blank UMETA(DisplayName = "Blank")
};