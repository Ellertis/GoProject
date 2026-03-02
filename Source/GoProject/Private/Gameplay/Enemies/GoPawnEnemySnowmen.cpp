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
	
	//Target tile out of grid
	if (!TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y)) {Intent.NewDirection = GetOppositeDirection();return Intent;}

	int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
	AGoTile* TargetTile = TileManager->Tiles[TargetIndex];
	
	//Target tile not walkable or occupied
	if (!TargetTile || !TargetTile->Walkable || EnemyManager->IsTileOccupied(TargetIndex, this)) {Intent.NewDirection = GetOppositeDirection();return Intent;}
	
	//Set Target tile as it's valid
	Intent.TargetTile = TargetTile;
	
	//Beyond tile out of grid
	FIntPoint BeyondCoord = TargetCoord + GetDirectionDelta(Direction);
	if (!TileManager->IsValidIndex(BeyondCoord.X, BeyondCoord.Y)){Intent.NewDirection = GetOppositeDirection();return Intent;}

	//Beyond tile not walkable
	int BeyondIndex = TileManager->Get1DIndex(BeyondCoord.X, BeyondCoord.Y);
	AGoTile* BeyondTile = TileManager->Tiles[BeyondIndex];
	if (!BeyondTile || !BeyondTile->Walkable)
	{
		Intent.NewDirection = GetOppositeDirection();
	}

	return Intent;
}

void AGoPawnEnemySnowmen::ApplyMoveIntent_Implementation(const FEnemyMoveIntent& Intent)
{
	Super::ApplyMoveIntent_Implementation(Intent);
}

void AGoPawnEnemySnowmen::OnPostMove_Implementation()
{
	
}