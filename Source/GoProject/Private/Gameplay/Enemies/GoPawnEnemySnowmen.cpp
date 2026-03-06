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
	
	UE_LOG(LogTemp, Display, TEXT("========== SNOWMAN DEBUG =========="));
	UE_LOG(LogTemp, Display, TEXT("Snowman at tile %d, grid (%d,%d)"), 
		CurrTile->Index, CurrCoord.X, CurrCoord.Y);
	UE_LOG(LogTemp, Display, TEXT("Current direction: %d (%s)"), 
		(int)Direction, 
		Direction == EFaceDirection::Xplus ? TEXT("X+") :
		Direction == EFaceDirection::Xminus ? TEXT("X-") :
		Direction == EFaceDirection::Yplus ? TEXT("Y+") : TEXT("Y-"));
	
	// Check if next tile is valid
	UE_LOG(LogTemp, Display, TEXT("Checking target tile at (%d,%d)"), TargetCoord.X, TargetCoord.Y);
	
	if (!TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y)) 
	{
		UE_LOG(LogTemp, Display, TEXT("→ Target tile OUT OF BOUNDS"));
		UE_LOG(LogTemp, Display, TEXT("→ Turning around to %d"), (int)GetOppositeDirection(MoveDir));
		Intent.NewDirection = GetOppositeDirection(MoveDir);
		return Intent;
	}

	int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
	AGoTile* TargetTilePtr = TileManager->Tiles[TargetIndex];
	
	UE_LOG(LogTemp, Display, TEXT("Target tile %d: Walkable=%d, Occupied=%d, Connected=%d"), 
		TargetIndex,
		TargetTilePtr ? TargetTilePtr->Walkable : 0,
		EnemyManager ? EnemyManager->IsTileOccupied(TargetIndex, this) : 0,
		TileManager->AreConnected(CurrTile->Index, TargetIndex));
	
	// Check if next tile is walkable and not occupied, not connected
	if (!TargetTilePtr || !TargetTilePtr->Walkable || 
		(EnemyManager && EnemyManager->IsTileOccupied(TargetIndex, this)) ||
		!TileManager->AreConnected(CurrTile->Index, TargetIndex)) 
	{
		UE_LOG(LogTemp, Display, TEXT("→ Target tile BLOCKED"));
		UE_LOG(LogTemp, Display, TEXT("→ Turning around to %d"), (int)GetOppositeDirection(MoveDir));
		Intent.NewDirection = GetOppositeDirection(MoveDir);
		return Intent;
	}
	
	UE_LOG(LogTemp, Display, TEXT("→ Target tile VALID - can move"));
	Intent.TargetTile = TargetTilePtr;
    
	// Check the beyond tile to decide if we should turn around next turn
	FIntPoint BeyondCoord = TargetCoord + GetDirectionDelta(Direction);
	UE_LOG(LogTemp, Display, TEXT("Checking beyond tile at (%d,%d)"), BeyondCoord.X, BeyondCoord.Y);
	
	if (TileManager->IsValidIndex(BeyondCoord.X, BeyondCoord.Y))
	{
		int BeyondIndex = TileManager->Get1DIndex(BeyondCoord.X, BeyondCoord.Y);
		AGoTile* BeyondTile = TileManager->Tiles[BeyondIndex];
		
		UE_LOG(LogTemp, Display, TEXT("Beyond tile %d: Walkable=%d, Occupied=%d, Connected=%d"), 
			BeyondIndex,
			BeyondTile ? BeyondTile->Walkable : 0,
			EnemyManager ? EnemyManager->IsTileOccupied(BeyondIndex, this) : 0,
			TileManager->AreConnected(TargetIndex, BeyondIndex));
		
		// If beyond tile is invalid/blocked, turn around after moving not connected
		if (!BeyondTile || !BeyondTile->Walkable || 
			(EnemyManager && EnemyManager->IsTileOccupied(BeyondIndex, this)) ||
			!TileManager->AreConnected(TargetIndex, BeyondIndex))
		{
			UE_LOG(LogTemp, Display, TEXT("→ Beyond tile BLOCKED - will turn around AFTER moving"));
			UE_LOG(LogTemp, Display, TEXT("→ New direction will be: %d"), (int)GetOppositeDirection(MoveDir));
			Intent.NewDirection = GetOppositeDirection(MoveDir);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("→ Beyond tile VALID - continuing straight"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("→ Beyond tile OUT OF BOUNDS - will turn around AFTER moving"));
		UE_LOG(LogTemp, Display, TEXT("→ New direction will be: %d"), (int)GetOppositeDirection(MoveDir));
		Intent.NewDirection = GetOppositeDirection(MoveDir);
	}

	UE_LOG(LogTemp, Display, TEXT("→ FINAL: Moving to tile %d, facing %d after move"), 
		Intent.TargetTile->Index, (int)Intent.NewDirection);
	UE_LOG(LogTemp, Display, TEXT("====================================="));

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