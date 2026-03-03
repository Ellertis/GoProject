// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GoPawn.h"
#include "Gameplay/Tile/GoTile.h"

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