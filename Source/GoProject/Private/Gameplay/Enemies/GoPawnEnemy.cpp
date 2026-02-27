// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "Gameplay/Tile/GoTileManager.h"


void AGoPawnEnemy::EnemyMovement()
{
	if(!TileManager) return;
	
	int CurrTileIndex = TileManager->Tiles.IndexOfByKey(CurrTile);
	if (CurrTileIndex == INDEX_NONE){UE_LOG(LogTemp, Error, TEXT("PawnEnemy: Current tile not in grid!"))return;}

	FIntPoint CurrTileCoord = TileManager->Get2DIndex(CurrTileIndex);
	FIntPoint TargetTileCoord = CurrTileCoord+GetDirectionDelta();
	
	int TargetTileIndex = TileManager->Get1DIndex(TargetTileCoord.X, TargetTileCoord.Y);
	AGoTile* TargetTile = TileManager->Tiles[TargetTileIndex];

	if(!TargetTile){UE_LOG(LogTemp, Error, TEXT("PawnEnemy: Target Tile invalid after fetching"))return;}
	if(!TargetTile->Walkable)return;

	SetActorLocation(TargetTile->GetActorLocation()+FVector(0,0,100));
	CurrTile = TargetTile;

	//Checking if the next tile is walkable, if not, rotate 180 and inverse Direction
	CurrTileCoord = TargetTileCoord;
	
	TargetTileCoord = CurrTileCoord+GetDirectionDelta();
	if(!TileManager->IsValidIndex(TargetTileCoord.X, TargetTileCoord.Y))
	{
		//Invalid Target Tile
		Direction = GetOppositeDirection(); // New direction (reversed direction)
		AddActorLocalRotation(FRotator(0,180,0));
		UE_LOG(LogTemp,Warning,TEXT("InvalidTargetTile"));
		//ReverseEnemyMovement(CurrTileCoord);
	}
	
	FinishTurn();
	
}

void AGoPawnEnemy::ReverseEnemyMovement(FIntPoint CurrTileCoord)
{
	if(!TileManager) return;
	FIntPoint TargetTileCoord = CurrTileCoord+GetDirectionDelta();
	
	if(!TileManager->IsValidIndex(TargetTileCoord.X, TargetTileCoord.Y))
	{
		//Invalid Target Tile
		UE_LOG(LogTemp,Warning,TEXT("Reverse InvalidTargetTile"));
		FinishTurn();
		return;
	}

	int TargetTileIndex = TileManager->Get1DIndex(TargetTileCoord.X, TargetTileCoord.Y);
	AGoTile* TargetTile = TileManager->Tiles[TargetTileIndex];

	if(!TargetTile){UE_LOG(LogTemp, Error, TEXT("PawnEnemy: Target Tile invalid after fetching"))return;}
	if(!TargetTile->Walkable)return;

	SetActorLocation(TargetTile->GetActorLocation()+FVector(0,0,TileManager->ZOffset*2));
	CurrTile = TargetTile;
	
	FinishTurn();
	
}

FIntPoint AGoPawnEnemy::GetDirectionDelta() const
{
	switch (Direction)
	{
		case EFaceDirection::Xplus: return FIntPoint(1,0);
		case EFaceDirection::Xminus: return FIntPoint(-1,0);
		case EFaceDirection::Yplus: return FIntPoint(0,1);
		case EFaceDirection::Yminus: return FIntPoint(0,-1);
		default: return FIntPoint(0,0);
	}
}

EFaceDirection AGoPawnEnemy::GetOppositeDirection() const
{
	switch (Direction){
		case EFaceDirection::Xplus: return EFaceDirection::Xminus;
		case EFaceDirection::Xminus: return EFaceDirection::Xplus;
		case EFaceDirection::Yplus: return EFaceDirection::Yminus;
		case EFaceDirection::Yminus: return EFaceDirection::Yplus;
		default: return EFaceDirection::Xplus;
	}
}

void AGoPawnEnemy::StartTurn()
{
	EnemyMovement();
}

void AGoPawnEnemy::FinishTurn()
{
	OnEnemyMovement.Broadcast();
}
