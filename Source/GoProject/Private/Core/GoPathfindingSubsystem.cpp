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

bool UGoPathfindingSubsystem::IsTileValidForFlee(int TileIndex, int PlayerIndex, int IgnoreIndex)
{
    if (!TileManager || !TileManager->Tiles.IsValidIndex(TileIndex)) return false;
    
    AGoTile* Tile = TileManager->Tiles[TileIndex];
    if (!Tile || !Tile->Walkable) return false;
    
    if (TileIndex == PlayerIndex) return false;
    if (TileIndex == IgnoreIndex) return false;
    
    return true;
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

TArray<int> UGoPathfindingSubsystem::FindDirectionalFleePath(int StartIndex, EFaceDirection PreferredDir, int PlayerIndex)
{
    if (!TileManager) return TArray<int>();
    
    TSet<int> ClosedSet;
    TMap<int, int> CameFrom;
    TMap<int, float> GScore;
    TMap<int, float> FScore;
    
    TArray<int> OpenSet;
    
    GScore.Add(StartIndex, 0);
    FScore.Add(StartIndex, Heuristic(StartIndex, PlayerIndex) * -1);
    OpenSet.Add(StartIndex);
    
    FIntPoint StartPos = TileManager->Get2DIndex(StartIndex);
    FIntPoint DirVec = DirectionVectors[PreferredDir];
    
    int Iterations = 0;
    const int MaxIterations = 100;
    
    while (OpenSet.Num() > 0 && Iterations < MaxIterations)
    {
        Iterations++;
        
        int Current = -1;
        float LowestF = MAX_FLT;
        for (int Index : OpenSet)
        {
            float F = FScore.Contains(Index) ? FScore[Index] : MAX_FLT;
            if (F < LowestF)
            {
                LowestF = F;
                Current = Index;
            }
        }
        
        if (Current == -1) break;
        
        OpenSet.Remove(Current);
        ClosedSet.Add(Current);
        
        float PlayerDist = Heuristic(Current, PlayerIndex);
        if (PlayerDist >= 5.0f)
        {
            return ReconstructPath(CameFrom, Current);
        }
        
        AGoTile* CurrentTile = TileManager->Tiles[Current];
        if (!CurrentTile) continue;
        
        for (int NeighborIndex : CurrentTile->Neighbors)
        {
            if (!TileManager->AreConnected(Current, NeighborIndex)) continue;
            if (!IsTileValidForFlee(NeighborIndex, PlayerIndex, StartIndex)) continue;
            if (ClosedSet.Contains(NeighborIndex)) continue;
            
            FIntPoint NeighborPos = TileManager->Get2DIndex(NeighborIndex);
            float DirectionBias = 0;
            
            FIntPoint Delta = NeighborPos - StartPos;
            if (FMath::Sign(Delta.X) == DirVec.X && FMath::Sign(Delta.Y) == DirVec.Y)
            {
                DirectionBias = -2.0f;
            }
            
            float TentativeGScore = GScore[Current] + 1.0f + DirectionBias;
            
            if (!OpenSet.Contains(NeighborIndex))
            {
                OpenSet.Add(NeighborIndex);
            }
            else if (TentativeGScore >= (GScore.Contains(NeighborIndex) ? GScore[NeighborIndex] : MAX_FLT))
            {
                continue;
            }
            
            CameFrom.Add(NeighborIndex, Current);
            GScore.Add(NeighborIndex, TentativeGScore);
            
            PlayerDist = Heuristic(NeighborIndex, PlayerIndex);
            FScore.Add(NeighborIndex, -PlayerDist + GScore[NeighborIndex] * 0.1f);
        }
    }
    
    return TArray<int>();
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
