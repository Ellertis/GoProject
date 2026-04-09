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
		return Intent;
	}

	FIntPoint CurrCoord = TileManager->Get2DIndex(CurrTile->Index);
	FIntPoint TargetCoord = CurrCoord + GetDirectionDelta(Direction);
	EFaceDirection MoveDir = Direction;
	
	bool bForwardValid = false;
	AGoTile* ForwardTile = nullptr;
	
	if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
	{
		int ForwardIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
		ForwardTile = TileManager->Tiles[ForwardIndex];
		
		if (ForwardTile && ForwardTile->Walkable && 
			TileManager->AreConnected(CurrTile->Index, ForwardIndex) &&
			(!EnemyManager || !EnemyManager->IsTileOccupied(ForwardIndex, this)))
		{
			bForwardValid = true;
		}
	}
	
	if (!bForwardValid)
	{
		EFaceDirection OppositeDir = GetOppositeDirection(MoveDir);
		FIntPoint BackwardCoord = CurrCoord + GetDirectionDelta(OppositeDir);
		
		if (TileManager->IsValidIndex(BackwardCoord.X, BackwardCoord.Y))
		{
			int BackwardIndex = TileManager->Get1DIndex(BackwardCoord.X, BackwardCoord.Y);
			AGoTile* BackwardTile = TileManager->Tiles[BackwardIndex];
			
			if (BackwardTile && BackwardTile->Walkable && 
				TileManager->AreConnected(CurrTile->Index, BackwardIndex) &&
				(!EnemyManager || !EnemyManager->IsTileOccupied(BackwardIndex, this)))
			{
				Intent.TargetTile = BackwardTile;
				Intent.NewDirection = OppositeDir;
				return Intent;
			}
		}
		Intent.NewDirection = OppositeDir;
		return Intent;
	}
	Intent.TargetTile = ForwardTile;
	FIntPoint BeyondCoord = TargetCoord + GetDirectionDelta(Direction);
	
	if (TileManager->IsValidIndex(BeyondCoord.X, BeyondCoord.Y))
	{
		int BeyondIndex = TileManager->Get1DIndex(BeyondCoord.X, BeyondCoord.Y);
		AGoTile* BeyondTile = TileManager->Tiles[BeyondIndex];
		int ForwardIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
		
		if (!BeyondTile || !BeyondTile->Walkable || 
			!TileManager->AreConnected(ForwardIndex, BeyondIndex) ||
			(EnemyManager && EnemyManager->IsTileOccupied(BeyondIndex, this)))
		{
			Intent.NewDirection = GetOppositeDirection(MoveDir);
		}
	}
	else{Intent.NewDirection = GetOppositeDirection(MoveDir);}
	
	return Intent;
}

void AGoPawnEnemySnowmen::ApplyMoveIntent_Implementation(const FMoveIntent& Intent)
{
	if (Intent.NewDirection != Direction)
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
		Direction = Intent.NewDirection;
	}
	
	if (Intent.TargetTile && Intent.TargetTile != CurrTile)
	{
		StartMoveToTile(Intent.TargetTile, MoveDuration);
	}
}

void AGoPawnEnemySnowmen::OnPostMove_Implementation()
{
}