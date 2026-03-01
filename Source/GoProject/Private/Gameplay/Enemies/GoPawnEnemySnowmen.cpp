// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Gameplay/Tile/GoTileManager.h"

FEnemyMoveIntent AGoPawnEnemySnowmen::ComputeMoveIntent_Implementation() const
{
	FEnemyMoveIntent Intent;
	Intent.TargetTile = CurrTile;
	Intent.NewDirection = Direction;

	if (!TileManager || !CurrTile) return Intent;

	FIntPoint CurrCoord = TileManager->Get2DIndex(CurrTile->Index);
	FIntPoint TargetCoord = CurrCoord + GetDirectionDelta(Direction);
	
	// if target tile is out of grid, rotate
	if (!TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y)){Intent.NewDirection = GetOppositeDirection();return Intent;}

	int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
	AGoTile* TargetTile = TileManager->Tiles[TargetIndex];
	// if target tile is not walkable or occupied, rotate
	if (!TargetTile || !TargetTile->Walkable || EnemyManager->IsTileOccupied(TargetIndex,this)){Intent.NewDirection = GetOppositeDirection(); return Intent;}
	
	Intent.TargetTile = TargetTile;
	FIntPoint BeyondCoord = TargetCoord + GetDirectionDelta(Direction);
	// if beyond tile is out of grid rotate when moving to the next tile
	if (!TileManager->IsValidIndex(BeyondCoord.X, BeyondCoord.Y)){Intent.NewDirection = GetOppositeDirection(); return Intent;}

	int BeyondIndex = TileManager->Get1DIndex(BeyondCoord.X, BeyondCoord.Y);
	AGoTile* BeyondTile = TileManager->Tiles[BeyondIndex];
	// if beyond tile is unwalkable rotate when moving to the next tile
	if (!BeyondTile || !BeyondTile->Walkable)Intent.NewDirection = GetOppositeDirection();

	return Intent;
}

void AGoPawnEnemySnowmen::ApplyMoveIntent_Implementation(const FEnemyMoveIntent& Intent)
{
	Super::ApplyMoveIntent_Implementation(Intent);
}

void AGoPawnEnemySnowmen::OnPostMove_Implementation()
{
	
}