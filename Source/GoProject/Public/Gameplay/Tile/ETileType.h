// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETileType.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Option1 UMETA(DisplayName = "Option1"),
	Option2 UMETA(DisplayName = "Option2"),
	Option3 UMETA(DisplayName = "Hidden")
};