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

float UGoPathfindingSubsystem::Heuristic(int TileA, int TileB)
{
    if (!TileManager) 
    {
        TileManager = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGoTileManager::StaticClass()));
        if (!TileManager) return MAX_FLT;
    }
    
    FIntPoint PosA = TileManager->Get2DIndex(TileA);
    FIntPoint PosB = TileManager->Get2DIndex(TileB);
    
    return FMath::Abs(PosA.X - PosB.X) + FMath::Abs(PosA.Y - PosB.Y);
}

TArray<int> UGoPathfindingSubsystem::FindFleePath(int StartIndex, int PlayerIndex, int MaxDistance)
{
    if (!TileManager) 
    {
        TileManager = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGoTileManager::StaticClass()));
        if (!TileManager) return TArray<int>();
    }
    
    TArray<int> BestPath;
    float BestScore = -MAX_FLT;
    
    TArray<EFaceDirection> Directions = {
        EFaceDirection::Xplus, EFaceDirection::Xminus,
        EFaceDirection::Yplus, EFaceDirection::Yminus
    };
    
    for (EFaceDirection Dir : Directions)
    {
        TArray<int> Path = FindDirectionalFleePath(StartIndex, Dir, PlayerIndex);
        if (Path.Num() > 0)
        {
            float PlayerDistance = Heuristic(Path.Last(), PlayerIndex);
            float Score = PlayerDistance - (Path.Num() * 0.5f); //Player proximity multiplier
            
            if (Score > BestScore)
            {
                BestScore = Score;
                BestPath = Path;
            }
        }
    }
    
    return BestPath;
}

TArray<int> UGoPathfindingSubsystem::FindDirectionalFleePath(int StartIndex, EFaceDirection PreferredDir, int PlayerIndex, AGoEnemyManager* EnemyManager)
{
    if (!TileManager) return TArray<int>();
    
    FIntPoint StartPos = TileManager->Get2DIndex(StartIndex);
    FIntPoint DirVec = DirectionVectors[PreferredDir];
    FIntPoint FirstTilePos = StartPos + DirVec;
    
    // First, try simple direct move
    int FirstTileIndex = -1;
    if (TileManager->IsValidIndex(FirstTilePos.X, FirstTilePos.Y))
    {
        FirstTileIndex = TileManager->Get1DIndex(FirstTilePos.X, FirstTilePos.Y);
        AGoTile* FirstTile = TileManager->Tiles[FirstTileIndex];
        
        if (FirstTile && FirstTile->Walkable && 
            TileManager->AreConnected(StartIndex, FirstTileIndex) &&
            FirstTileIndex != PlayerIndex &&
            (!EnemyManager || !EnemyManager->IsTileOccupied(FirstTileIndex, nullptr)))
        {
            // Return a path with just the first move
            TArray<int> SimplePath;
            SimplePath.Add(StartIndex);
            SimplePath.Add(FirstTileIndex);
            return SimplePath;
        }
    }
    
    // A* to find the farthest reachable tile
    TMap<int, int> CameFrom;
    TMap<int, float> GScore;
    TSet<int> ClosedSet;
    TArray<int> OpenSet;
    
    GScore.Add(StartIndex, 0);
    OpenSet.Add(StartIndex);
    
    int BestTile = StartIndex;
    float BestDistance = Heuristic(StartIndex, PlayerIndex);
    
    int Iterations = 0;
    const int MaxIterations = 200;
    
    while (OpenSet.Num() > 0 && Iterations < MaxIterations)
    {
        Iterations++;
        
        // Find node with lowest GScore
        int Current = -1;
        float LowestG = MAX_FLT;
        for (int Index : OpenSet)
        {
            float G = GScore.Contains(Index) ? GScore[Index] : MAX_FLT;
            if (G < LowestG)
            {
                LowestG = G;
                Current = Index;
            }
        }
        
        if (Current == -1) break;
        
        OpenSet.Remove(Current);
        ClosedSet.Add(Current);
        
        float CurrentPlayerDist = Heuristic(Current, PlayerIndex);
        
        if (CurrentPlayerDist > BestDistance)
        {
            BestDistance = CurrentPlayerDist;
            BestTile = Current;
        }
        
        AGoTile* CurrentTile = TileManager->Tiles[Current];
        if (!CurrentTile) continue;
        
        for (int NeighborIndex : CurrentTile->Neighbors)
        {
            if (!TileManager->AreConnected(Current, NeighborIndex)) continue;
            if (ClosedSet.Contains(NeighborIndex)) continue;
            
            AGoTile* NeighborTile = TileManager->Tiles[NeighborIndex];
            if (!NeighborTile || !NeighborTile->Walkable) continue;
            if (NeighborIndex == PlayerIndex) continue;
            if (EnemyManager && EnemyManager->IsTileOccupied(NeighborIndex, nullptr)) continue;
            
            float MoveCost = 1.0f;
            
            FIntPoint NeighborPos = TileManager->Get2DIndex(NeighborIndex);
            FIntPoint Delta = NeighborPos - StartPos;
            if (FMath::Sign(Delta.X) == DirVec.X && FMath::Sign(Delta.Y) == DirVec.Y)
            {
                MoveCost -= 0.5f; // Lower cost = more preferred
            }
            
            // Penalty for moving towards player
            float NewPlayerDist = Heuristic(NeighborIndex, PlayerIndex);
            if (NewPlayerDist < CurrentPlayerDist)
            {
                MoveCost += 10.0f;
            }
            
            float TentativeGScore = GScore[Current] + MoveCost;
            
            if (!GScore.Contains(NeighborIndex) || TentativeGScore < GScore[NeighborIndex])
            {
                CameFrom.Add(NeighborIndex, Current);
                GScore.Add(NeighborIndex, TentativeGScore);
                
                if (!OpenSet.Contains(NeighborIndex))
                {
                    OpenSet.Add(NeighborIndex);
                }
            }
        }
    }
    
    // Reconstruct path to the best tile
    if (BestTile != StartIndex && CameFrom.Contains(BestTile))
    {
        TArray<int> Path;
        int Current = BestTile;
        while (Current != StartIndex)
        {
            Path.Insert(Current, 0);
            if (CameFrom.Contains(Current))
            {
                Current = CameFrom[Current];
            }
            else
            {
                break;
            }
        }
        Path.Insert(StartIndex, 0);
        
        if (Path.Num() > 1)
        {
            return Path;
        }
    }
    
    // Fallback: return the first move if available
    if (FirstTileIndex != -1)
    {
        TArray<int> SimplePath;
        SimplePath.Add(StartIndex);
        SimplePath.Add(FirstTileIndex);
        return SimplePath;
    }
    
    return TArray<int>();
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

TArray<int> UGoPathfindingSubsystem::ReconstructPath(const TMap<int, int>& CameFrom, int Current)
{
    TArray<int> Path;
    Path.Add(Current);
    
    int CurrentNode = Current;
    while (CameFrom.Contains(CurrentNode))
    {
        CurrentNode = CameFrom[CurrentNode];
        Path.Insert(CurrentNode, 0);
    }
    
    return Path;
}
