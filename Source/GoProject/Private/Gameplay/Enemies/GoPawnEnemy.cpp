// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Gameplay/Tile/GoTile.h"

AGoPawnEnemy::AGoPawnEnemy()
{
    Direction = EFaceDirection::Xplus;
}

void AGoPawnEnemy::PreTurnUpdate_Implementation()
{
	
}

FMoveIntent AGoPawnEnemy::ComputeMoveIntent_Implementation() const
{
    FMoveIntent Intent;
    Intent.TargetTile = CurrTile; 
    Intent.NewDirection = Direction;
    return Intent;
}

void AGoPawnEnemy::ApplyMoveIntent_Implementation(const FMoveIntent& Intent)
{
    if (!Intent.TargetTile) return;
    
    if(Direction != Intent.NewDirection)
    {
        PendingMoveTile = Intent.TargetTile;
        StartRotationToDirection(Intent.NewDirection, RotationDuration);
        return;
    }
    /*
    //Rotation logic starts here 
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
    */
	StartMoveToTile(Intent.TargetTile, MoveDuration);
	/*
	OnMoveToTile(Intent.TargetTile);
    OnMoveEnd();
	 */
}

void AGoPawnEnemy::OnPostMove_Implementation()
{
    // Base implementation
}

void AGoPawnEnemy::ApplyDamage_Implementation(int Amount, EFaceDirection HitDirection)
{
    OnDamageTaken(Amount, HitDirection);
}

bool AGoPawnEnemy::CanMoveToTile(AGoTile* Tile) const
{
    if (!Super::CanMoveToTile(Tile)) return false;
    if (!EnemyManager || !Tile) return true;

    return !EnemyManager->IsTileOccupied(Tile->Index, this);
}

bool AGoPawnEnemy::CanMoveToTileIndex(int TileIndex) const
{
    AGoTile* Tile = GetTileFromIndex(TileIndex);
    return CanMoveToTile(Tile);
}