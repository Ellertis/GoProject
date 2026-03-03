// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "GoPathfindingSubsystem.generated.h"

class AGoTileManager;
class AGoTile;

USTRUCT()
struct FPathNode
{
	GENERATED_BODY()
    
	int TileIndex;
	float GCost;
	float HCost;
	float FCost() const { return GCost + HCost; }
	int ParentIndex;
    
	FPathNode() : TileIndex(-1), GCost(0), HCost(0), ParentIndex(-1) {}
	FPathNode(int InIndex) : TileIndex(InIndex), GCost(0), HCost(0), ParentIndex(-1) {}
};

UCLASS()
class GOPROJECT_API UGoPathfindingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	TArray<int> FindFleePath(int StartIndex, int PlayerIndex, int MaxDistance = 10);
	
	UFUNCTION(BlueprintCallable)
	TArray<int> FindDirectionalFleePath(int StartIndex, EFaceDirection HitDirection, int PlayerIndex);

private:
	UPROPERTY()
	AGoTileManager* TileManager;
    
	float Heuristic(int TileA, int TileB);
	TArray<int> ReconstructPath(const TMap<int, int>& CameFrom, int Current);
	bool IsTileValidForFlee(int TileIndex, int PlayerIndex, int IgnoreIndex = -1);
    
	TMap<EFaceDirection, FIntPoint> DirectionVectors;
};