// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Gameplay/Tile/GoTileManager.h"

FMoveIntent AGoPawnEnemySnowmen::ComputeMoveIntent_Implementation() const
{
	FMoveIntent Intent;
	Intent.TargetTile = CurrTile;
	Intent.NewDirection = Direction;

	if (!TileManager || !CurrTile) 
	{
		UE_LOG(LogTemp, Error, TEXT("Snowman: Missing TileManager or CurrTile"));
		return Intent;
	}

	FIntPoint CurrCoord = TileManager->Get2DIndex(CurrTile->Index);
	FIntPoint TargetCoord = CurrCoord + GetDirectionDelta(Direction);
	EFaceDirection MoveDir = Direction;
	
	// Check if next tile is valid
	if (!TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y)) 
	{
		Intent.NewDirection = GetOppositeDirection(MoveDir);
		return Intent;
	}

	int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
	AGoTile* TargetTilePtr = TileManager->Tiles[TargetIndex];
	
	// Check if next tile is walkable and not occupied, not connected
	if (!TargetTilePtr || !TargetTilePtr->Walkable || 
		(EnemyManager && EnemyManager->IsTileOccupied(TargetIndex, this)) ||
		!TileManager->AreConnected(CurrTile->Index, TargetIndex)) 
	{
		Intent.NewDirection = GetOppositeDirection(MoveDir);
		return Intent;
	}
	
	Intent.TargetTile = TargetTilePtr;
    
	// Check the beyond tile to decide if we should turn around next turn
	FIntPoint BeyondCoord = TargetCoord + GetDirectionDelta(Direction);
	
	if (TileManager->IsValidIndex(BeyondCoord.X, BeyondCoord.Y))
	{
		int BeyondIndex = TileManager->Get1DIndex(BeyondCoord.X, BeyondCoord.Y);
		AGoTile* BeyondTile = TileManager->Tiles[BeyondIndex];
		
		// If beyond tile is invalid/blocked, turn around after moving not connected
		if (!BeyondTile || !BeyondTile->Walkable || 
			(EnemyManager && EnemyManager->IsTileOccupied(BeyondIndex, this)) ||
			!TileManager->AreConnected(TargetIndex, BeyondIndex))
		{
			Intent.NewDirection = GetOppositeDirection(MoveDir);
		}
	}
	else
	{
		Intent.NewDirection = GetOppositeDirection(MoveDir);
	}

	return Intent;
}

void AGoPawnEnemySnowmen::ApplyMoveIntent_Implementation(const FMoveIntent& Intent)
{
	UE_LOG(LogTemp, Display, TEXT("Snowman APPLYING move:"));
	UE_LOG(LogTemp, Display, TEXT("  From tile %d to %d"), 
		CurrTile ? CurrTile->Index : -1,
		Intent.TargetTile ? Intent.TargetTile->Index : -1);
	UE_LOG(LogTemp, Display, TEXT("  Direction changing: %d -> %d"), 
		(int)Direction, (int)Intent.NewDirection);
	
	// Store the target for the movement system
	if (Intent.TargetTile && Intent.TargetTile != CurrTile)
	{
		FRotator NewRotation = FRotator::ZeroRotator;
		switch (Intent.NewDirection)
		{
			case EFaceDirection::Xplus:  NewRotation = FRotator(0, 0, 0); break;
			case EFaceDirection::Xminus: NewRotation = FRotator(0, 180, 0); break;
			case EFaceDirection::Yplus:  NewRotation = FRotator(0, 90, 0); break;
			case EFaceDirection::Yminus: NewRotation = FRotator(0, -90, 0); break;
		}
		SetActorRotation(NewRotation);
		
		//Direction = Intent.NewDirection;
		NewDirection = Intent.NewDirection;
		
		// Start the animated movement
		StartMoveToTile(Intent.TargetTile, MoveDuration);
	}
}

void AGoPawnEnemySnowmen::OnPostMove_Implementation()
{
	UE_LOG(LogTemp, Display, TEXT("Snowman OnPostMove at tile %d"), 
		CurrTile ? CurrTile->Index : -1);
}