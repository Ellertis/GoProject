// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GoPathfindingSubsystem.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "Gameplay/Tile/GoTile.h"
#include "Kismet/GameplayStatics.h"

void UGoPathfindingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    DirectionVectors.Add(EFaceDirection::Xplus, FIntPoint(1, 0));
    DirectionVectors.Add(EFaceDirection::Xminus, FIntPoint(-1, 0));
    DirectionVectors.Add(EFaceDirection::Yplus, FIntPoint(0, 1));
    DirectionVectors.Add(EFaceDirection::Yminus, FIntPoint(0, -1));
}

void UGoPathfindingSubsystem::Deinitialize()
{
    TileManager = nullptr;
    Super::Deinitialize();
}

int UGoPathfindingSubsystem::CountReachableTilesInDirection(int StartIndex, EFaceDirection Direction, int PlayerIndex, AGoEnemyManager* EnemyManager, TArray<int>& OutReachableTiles, float& OutAvgDistance)
{
    if (!TileManager) 
    {
        if (GetWorld()){TileManager = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGoTileManager::StaticClass()));}
        if (!TileManager)
        {
            return 0;
        }
    }
    
    FIntPoint StartPos = TileManager->Get2DIndex(StartIndex);
    FIntPoint PlayerPos = TileManager->Get2DIndex(PlayerIndex);
    FIntPoint PrimaryDir;
    switch (Direction)
    {
        case EFaceDirection::Xplus:  PrimaryDir = FIntPoint(1, 0); break; 
        case EFaceDirection::Xminus: PrimaryDir = FIntPoint(-1, 0); break;
        case EFaceDirection::Yplus:  PrimaryDir = FIntPoint(0, 1); break;
        case EFaceDirection::Yminus: PrimaryDir = FIntPoint(0, -1); break;
        default: PrimaryDir = FIntPoint(0, 0); break;
    }
    
    FIntPoint FirstTilePos = StartPos + PrimaryDir;
    if (!TileManager->IsValidIndex(FirstTilePos.X, FirstTilePos.Y)){return 0;}
    
    int FirstTileIndex = TileManager->Get1DIndex(FirstTilePos.X, FirstTilePos.Y);
    AGoTile* FirstTile = TileManager->Tiles[FirstTileIndex];
    bool bWalkable = FirstTile && FirstTile->Walkable;
    bool bConnected = TileManager->AreConnected(StartIndex, FirstTileIndex);
    bool bOccupied = EnemyManager && EnemyManager->IsTileOccupied(FirstTileIndex, nullptr);
    
    if (!bWalkable || !bConnected || bOccupied){return 0;}
    
    TSet<int> Visited;
    TArray<int> Queue;
    
    Visited.Add(StartIndex);
    Queue.Add(StartIndex);
    OutReachableTiles.Empty();
    OutAvgDistance = 0;
    
    FIntPoint PerpDir1, PerpDir2;
    if (PrimaryDir.X != 0)
    {
        PerpDir1 = FIntPoint(0, 1);
        PerpDir2 = FIntPoint(0, -1);
    }
    else
    {
        PerpDir1 = FIntPoint(1, 0);
        PerpDir2 = FIntPoint(-1, 0);
    }
    
    FIntPoint OppositeDir = FIntPoint(-PrimaryDir.X, -PrimaryDir.Y);
    TArray<FIntPoint> NeighborDirs = { PrimaryDir, OppositeDir, PerpDir1, PerpDir2 };
    
    int TileCount = 0;
    
    while (Queue.Num() > 0)
    {
        int CurrentIndex = Queue[0];
        Queue.RemoveAt(0);
        
        FIntPoint CurrentPos = TileManager->Get2DIndex(CurrentIndex);
        
        for (const FIntPoint& DirVec : NeighborDirs)
        {
            FIntPoint NeighborPos = CurrentPos + DirVec;
            
            if (!TileManager->IsValidIndex(NeighborPos.X, NeighborPos.Y)) continue;
            
            int NeighborIndex = TileManager->Get1DIndex(NeighborPos.X, NeighborPos.Y);
            
            if (Visited.Contains(NeighborIndex)) continue;
            
            AGoTile* NeighborTile = TileManager->Tiles[NeighborIndex];
            if (!NeighborTile || !NeighborTile->Walkable) {continue;}
            if (!TileManager->AreConnected(CurrentIndex, NeighborIndex)) {continue;}
            if (EnemyManager && EnemyManager->IsTileOccupied(NeighborIndex, nullptr)) {continue;}
            
            FIntPoint Delta = NeighborPos - StartPos;
            float Dot = (Delta.X * PrimaryDir.X) + (Delta.Y * PrimaryDir.Y);
            
            if (Dot < -0.5f) {continue;}
            
            Visited.Add(NeighborIndex);
            Queue.Add(NeighborIndex);
            
            TileCount++;
            OutReachableTiles.Add(NeighborIndex);
            float PlayerDist = FMath::Abs(NeighborPos.X - PlayerPos.X) + FMath::Abs(NeighborPos.Y - PlayerPos.Y);
            OutAvgDistance += PlayerDist;
        }
    }
    
    if (TileCount > 0)
    {
        OutAvgDistance /= TileCount;
    }
    
    return TileCount;
}
