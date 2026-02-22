// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GoTile.h"
#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "BoardDataAsset.generated.h"

USTRUCT()
struct FEnemySpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGoPawnEnemy> EnemyClass = nullptr;

	UPROPERTY(EditAnywhere)
	ETileConnection FaceDirection = ETileConnection::Xplus;
	
	UPROPERTY(EditAnywhere)
	int Count = 1;
};

USTRUCT()
struct FGoTileData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AGoTile> TileClass = nullptr;
    
	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "/Script/GoProject/ETileConnection"))
	int32 Connections = 0;

	UPROPERTY(EditAnywhere)
	ETileType TileType = ETileType::Empty;
    
	UPROPERTY(EditAnywhere)
	bool Walkable = true;

	UPROPERTY(EditAnywhere)
	TArray<FEnemySpawnData> Enemies;
	
};


UCLASS()
class GOPROJECT_API UBoardDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Displacement = 100;

	UPROPERTY(VisibleAnywhere)
	TArray<FGoTileData> Tiles;
	
};
