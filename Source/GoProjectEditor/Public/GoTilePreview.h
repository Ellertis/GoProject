// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "Gameplay/Tile/GoTile.h"
#include "GoTilePreview.generated.h"

/**
 * 
 */
UCLASS()
class GOPROJECTEDITOR_API AGoTilePreview : public AGoTile
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AGoTile> TileClass;

	UPROPERTY(EditAnywhere)
	TArray<FEnemySpawnData> Enemies;
};
