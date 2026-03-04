// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Gameplay/Tile/GoTileManager.h"

FMoveIntent AGoPawnEnemySnowmen::ComputeMoveIntent_Implementation() const
{
	// Snowman movement logic is relatively simple, advance to the next tile in the face direction if it's possible.
	// if snowman advances, check the tile after to see if it's walkable, if not rotate 180 degrees.
	FMoveIntent Intent;
	Intent.TargetTile = CurrTile;
	Intent.NewDirection = Direction;

	if (!TileManager || !CurrTile) return Intent;

	FIntPoint CurrCoord = TileManager->Get2DIndex(CurrTile->Index);
	FIntPoint TargetCoord = CurrCoord + GetDirectionDelta(Direction);
    
	if (!TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y)) 
	{
		Intent.NewDirection = GetOppositeDirection(Direction);
		return Intent;
	}

	int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
	AGoTile* TargetTilePtr = TileManager->Tiles[TargetIndex];
	if (!TargetTilePtr || !TargetTilePtr->Walkable || EnemyManager->IsTileOccupied(TargetIndex, this)) 
	{
		Intent.NewDirection = GetOppositeDirection(Direction);
		return Intent;
	}
    
	Intent.TargetTile = TargetTilePtr;
    
	FIntPoint BeyondCoord = TargetCoord + GetDirectionDelta(Direction);
	if (!TileManager->IsValidIndex(BeyondCoord.X, BeyondCoord.Y))
	{
		Intent.NewDirection = GetOppositeDirection(Direction);
		return Intent;
	}

	int BeyondIndex = TileManager->Get1DIndex(BeyondCoord.X, BeyondCoord.Y);
	AGoTile* BeyondTile = TileManager->Tiles[BeyondIndex];
	if (!BeyondTile || !BeyondTile->Walkable || EnemyManager->IsTileOccupied(BeyondIndex, this))
	{
		Intent.NewDirection = GetOppositeDirection(Direction);
	}

	return Intent;
}

void AGoPawnEnemySnowmen::ApplyMoveIntent_Implementation(const FMoveIntent& Intent)
{
	Super::ApplyMoveIntent_Implementation(Intent);
}

void AGoPawnEnemySnowmen::OnPostMove_Implementation()
{
}