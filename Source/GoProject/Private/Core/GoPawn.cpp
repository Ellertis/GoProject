// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GoPawn.h"

#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "Gameplay/Enemies/GoPawnEnemyGunter.h"
#include "Gameplay/Tile/GoTile.h"
#include "Player/GoPawnPlayer.h"

AGoPawn::AGoPawn()
{
    PrimaryActorTick.bCanEverTick = false;
    
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
}

void AGoPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AGoPawn::StartRotationToDirection(EFaceDirection NewDirection, float Duration)
{
    if (bIsMoving || bIsRotating) return;
	
    switch (NewDirection)
    {
        case EFaceDirection::Xplus:  TargetRotation = FRotator(0, 0, 0); break;
        case EFaceDirection::Xminus: TargetRotation = FRotator(0, 180, 0); break;
        case EFaceDirection::Yplus:  TargetRotation = FRotator(0, 90, 0); break;
        case EFaceDirection::Yminus: TargetRotation = FRotator(0, -90, 0); break;
        default: TargetRotation = FRotator(0, 0, 0); break;
    }
    
    StartRotation = GetActorRotation();
    RotationDuration = Duration;
    bIsRotating = true;
    RotationStartTime = GetWorld()->GetTimeSeconds();

	OnRotationStart();
	
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AGoPawn::UpdateRotation);
}

void AGoPawn::UpdateRotation()
{
    if (!bIsRotating) return;
    
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - RotationStartTime;
    float Alpha = FMath::Clamp(ElapsedTime / RotationDuration, 0.0f, 1.0f);
	
    FRotator NewRotation = FMath::Lerp(StartRotation, TargetRotation, Alpha);
    SetActorRotation(NewRotation);
    
    if (Alpha >= 1.0f){OnRotationComplete();}

	float NextUpdateTime = FMath::Min(0.016f, RotationDuration - ElapsedTime);
	GetWorld()->GetTimerManager().SetTimer(RotationTimerHandle, this, 
		&AGoPawn::UpdateRotation, NextUpdateTime, false);
}

void AGoPawn::OnRotationComplete()
{
    bIsRotating = false;
    GetWorld()->GetTimerManager().ClearTimer(RotationTimerHandle);
	
    SetActorRotation(TargetRotation);
	
    FIntPoint Delta;
    if (FMath::IsNearlyEqual(TargetRotation.Yaw, 0.0f)) Delta = FIntPoint(1, 0);
    else if (FMath::IsNearlyEqual(TargetRotation.Yaw, 180.0f)) Delta = FIntPoint(-1, 0);
    else if (FMath::IsNearlyEqual(TargetRotation.Yaw, 90.0f)) Delta = FIntPoint(0, 1);
    else if (FMath::IsNearlyEqual(TargetRotation.Yaw, -90.0f)) Delta = FIntPoint(0, -1);
    else Delta = FIntPoint(0, 0);
    
    Direction = GetDirectionFromDelta(Delta);
	OnRotationEnd();
	
    if (PendingMoveTile)
    {
        StartMoveToTile(PendingMoveTile, MoveDuration);
        PendingMoveTile = nullptr;
    }
}

void AGoPawn::StartMoveToTile(AGoTile* Tile, float Duration)
{
	
	if (!Tile || bIsMoving || !TileManager) return;
    
	TargetTile = Tile;
	MoveDuration = Duration;
	StartLocation = GetActorLocation();
	TargetLocation = GetTilePosition(Tile);
	MoveStartTime = GetWorld()->GetTimeSeconds();    
	
	if (CurrTile && Cast<AGoPawnPlayer>(this))
	{
		FIntPoint CurrentPos = TileManager->Get2DIndex(CurrTile->Index);
		FIntPoint TargetPos = TileManager->Get2DIndex(Tile->Index);
		FIntPoint Delta = TargetPos - CurrentPos;
		Direction = GetDirectionFromDelta(Delta);
	}
    
	bIsMoving = true;
	
	OnMoveStart();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this,&AGoPawn::UpdateMove);
	
}

void AGoPawn::UpdateMove()
{	
	if (!bIsMoving) return;
		
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - MoveStartTime;
	float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);
	
	FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

	if(MovementStyle == Jump)
	{
		float Height = GetJumpHeight_Implementation(Alpha);
		NewLocation.Z += Height;	
	}
	
	SetActorLocation(NewLocation);
    
	if (Alpha >= 1.0f) {FinishMove();}

	float NextUpdateTime = FMath::Min(0.016f, MoveDuration - ElapsedTime);
	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, 
		&AGoPawn::UpdateMove, NextUpdateTime, false);
}

void AGoPawn::FinishMove()
{
	if (!bIsMoving) return;
	
	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

	SetActorLocation(TargetLocation);
	CurrTile = TargetTile;
	bIsMoving = false;
	OnMoveEnd();
	OnMoveToTile(TargetTile);
	
	if (AGoPawnPlayer* Player = Cast<AGoPawnPlayer>(this))
	{
		Player->FinishTurn();
	}
	else if (AGoPawnEnemy* Enemy = Cast<AGoPawnEnemy>(this))
	{
		if(AGoPawnEnemyGunter* Gunter = Cast<AGoPawnEnemyGunter>(this)){if(Gunter->bWasHitThisTurn && EnemyManager){EnemyManager->OnGunterImmediateMoveCompleted();return;}} //Add On immideate move end
		if(EnemyManager){EnemyManager->OnEnemyMoveCompleted(Enemy);}
	}
}

float AGoPawn::GetJumpHeight_Implementation(float Alpha)
{
	// Parabolic curve
	return JumpHeight * 4.0f * Alpha * (1.0f - Alpha);
}

bool AGoPawn::CanMoveToTile(AGoTile* Tile) const
{
    if (!Tile || !TileManager || !CurrTile) return false;
    return Tile->Walkable && TileManager->AreConnected(CurrTile->Index, Tile->Index);
}

TArray<AGoTile*> AGoPawn::GetValidMoveTiles() const
{
    if (!TileManager || !CurrTile) return TArray<AGoTile*>();
    return TileManager->GetWalkableNeighbors(CurrTile->Index);
}

void AGoPawn::OnMoveStart_Implementation()
{
    // Blueprint implementation
}

void AGoPawn::OnMoveEnd_Implementation()
{
    // Blueprint implementation
}

void AGoPawn::OnMoveToTile_Implementation(AGoTile* Tile)
{
    if (!Tile) return;
    
    SetActorLocation(GetTilePosition(Tile));
    CurrTile = Tile;
}

void AGoPawn::OnRotationStart_Implementation()
{
	// Blueprint implementation
}

void AGoPawn::OnRotationEnd_Implementation()
{
	// Blueprint implementation
}

FIntPoint AGoPawn::GetDirectionDelta(EFaceDirection DirectionValue) const
{
    switch (DirectionValue)
    {
        case EFaceDirection::Xplus: return FIntPoint(1, 0);
        case EFaceDirection::Xminus: return FIntPoint(-1, 0);
        case EFaceDirection::Yplus: return FIntPoint(0, 1);
        case EFaceDirection::Yminus: return FIntPoint(0, -1);
        default: return FIntPoint(0, 0);
    }
}

EFaceDirection AGoPawn::GetDirectionFromDelta(const FIntPoint& Delta) const
{
    if (Delta.X > 0) return EFaceDirection::Xplus;
    if (Delta.X < 0) return EFaceDirection::Xminus;
    if (Delta.Y > 0) return EFaceDirection::Yplus;
    if (Delta.Y < 0) return EFaceDirection::Yminus;
    return Direction;
}

EFaceDirection AGoPawn::GetOppositeDirection(EFaceDirection Dir) const
{
    switch (Dir){
        case EFaceDirection::Xplus: return EFaceDirection::Xminus;
        case EFaceDirection::Xminus: return EFaceDirection::Xplus;
        case EFaceDirection::Yplus: return EFaceDirection::Yminus;
        case EFaceDirection::Yminus: return EFaceDirection::Yplus;
        default: return EFaceDirection::Xplus;
    }
}

FVector AGoPawn::GetTilePosition(AGoTile* Tile) const
{
    return Tile->GetActorLocation() + FVector(0, 0, HeightOffset);
}

AGoTile* AGoPawn::GetTileFromIndex(int TileIndex) const
{
    if (!TileManager || !TileManager->Tiles.IsValidIndex(TileIndex)) return nullptr;
    return TileManager->Tiles[TileIndex];
}