// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ETileType.h"
#include "TileStruct.generated.h"

USTRUCT(BlueprintType)
struct FTileStruct
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType = ETileType::Option1;
};