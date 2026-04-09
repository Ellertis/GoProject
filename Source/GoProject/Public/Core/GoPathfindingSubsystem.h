// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "GoPathfindingSubsystem.generated.h"

class AGoTileManager;
class AGoTile;

UCLASS()
class GOPROJECT_API UGoPathfindingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	int CountReachableTilesInDirection(int StartIndex, EFaceDirection Direction, int PlayerIndex, AGoEnemyManager* EnemyManager, TArray<int>& OutReachableTiles, float& OutAvgDistance);

private:
	UPROPERTY()
	AGoTileManager* TileManager;
    
	TMap<EFaceDirection, FIntPoint> DirectionVectors;
};