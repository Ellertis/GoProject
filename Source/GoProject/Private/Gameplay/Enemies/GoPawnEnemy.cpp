// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "Gameplay/Tile/GoTileManager.h"


void AGoPawnEnemy::PreTurnUpdate_Implementation()
{
	
}

FEnemyMoveIntent AGoPawnEnemy::ComputeMoveIntent_Implementation() const
{
	UE_LOG(LogTemp, Error, TEXT("AGoPawnEnemy::ComputeMoveIntent_Implementation called directly! Derived class must override this method."));

	FEnemyMoveIntent Intent;
	Intent.TargetTile = CurrTile; 
	Intent.NewDirection = Direction;
	return Intent;
}


void AGoPawnEnemy::ApplyMoveIntent_Implementation(const FEnemyMoveIntent& Intent)
{
	if (!Intent.TargetTile) return;
	
	float ZOffset = TileManager ? TileManager->ZOffset * 2 : 100.0f;
	SetActorLocation(Intent.TargetTile->GetActorLocation() + FVector(0, 0, ZOffset));
	
	CurrTile = Intent.TargetTile;
	
	Direction = Intent.NewDirection;
	FRotator NewRotation;
	switch (Direction)
	{
		case EFaceDirection::Xplus:  NewRotation = FRotator(0, 0, 0); break;
		case EFaceDirection::Xminus: NewRotation = FRotator(0, 180, 0); break;
		case EFaceDirection::Yplus:  NewRotation = FRotator(0, 90, 0); break;
		case EFaceDirection::Yminus: NewRotation = FRotator(0, -90, 0); break;
	}
	SetActorRotation(NewRotation);
}


void AGoPawnEnemy::OnPostMove_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("AGoPawnEnemy::OnPostMove_Implementation called directly! Derived class must override this method."));
}

void AGoPawnEnemy::ApplyDamage_Implementation(int Amount)
{
	UE_LOG(LogTemp, Error, TEXT("AGoPawnEnemy::TakeDamage_Implementation called directly! Derived class must override this method."));
}

FIntPoint AGoPawnEnemy::GetDirectionDelta(const EFaceDirection DirectionValue) const
{
	switch (DirectionValue)
	{
		case EFaceDirection::Xplus: return FIntPoint(1,0);
		case EFaceDirection::Xminus: return FIntPoint(-1,0);
		case EFaceDirection::Yplus: return FIntPoint(0,1);
		case EFaceDirection::Yminus: return FIntPoint(0,-1);
		default: return FIntPoint(0,0);
	}
}

EFaceDirection AGoPawnEnemy::GetDirectionFromDelta(FIntPoint& Delta) const
{
	if (Delta.X > 0) return EFaceDirection::Xplus;
	if (Delta.X < 0) return EFaceDirection::Xminus;
	if (Delta.Y > 0) return EFaceDirection::Yplus;
	if (Delta.Y < 0) return EFaceDirection::Yminus;
	UE_LOG(LogTemp,Error,TEXT("GoPawnEnemy Normally unreachable path reached, GetDirectionFromDelta input Delta is (0,0)"));
	return Direction;
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
