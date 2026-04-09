// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoPawnEnemyGunter.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "Core/GoPathfindingSubsystem.h"
#include "Player/GoPawnPlayer.h"
#include "Core/GoGameModeBase.h"

AGoPawnEnemyGunter::AGoPawnEnemyGunter()
{
    Health = StartHealth;
    bIsFleeing = false;
    MovementStyle = Slide;
}

void AGoPawnEnemyGunter::BeginPlay()
{
    Super::BeginPlay();
    
    AGoGameModeBase* GameModeBase = Cast<AGoGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GameModeBase) {PlayerRef = GameModeBase->GetPlayer();}
	
    PathfindingSubsystem = GetWorld()->GetSubsystem<UGoPathfindingSubsystem>();
}

void AGoPawnEnemyGunter::PreTurnUpdate_Implementation()
{
    if (!TileManager || !CurrTile || !PlayerRef || !PlayerRef->CurrTile)
        return;

    bWasHitThisTurn = false;
	
    FIntPoint GunterPos = TileManager->Get2DIndex(CurrTile->Index);
    FIntPoint PlayerPos = TileManager->Get2DIndex(PlayerRef->CurrTile->Index);
    int ManhattanDist = FMath::Abs(GunterPos.X - PlayerPos.X) + FMath::Abs(GunterPos.Y - PlayerPos.Y);
	
    if (ManhattanDist == 1)
    {
    	if (!TileManager->AreConnected(CurrTile->Index, PlayerRef->CurrTile->Index))
    	{
    		return;
    	}
        
        FIntPoint Delta = GunterPos - PlayerPos;
        EFaceDirection AwayDir = GetDirectionFromDelta(Delta);
        FIntPoint TargetCoord = GunterPos + GetDirectionDelta(AwayDir);
        
        if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
        {
            int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
            
            if (CanMoveToTileIndex(TargetIndex))
            {
                if(!bIsFleeing)
                {
                    if(EnemyManager){EnemyManager->bIsGunterPlayingFearAnimation = true;}
                    PlayFearAnimation();
                }
                StartFleeing(true, AwayDir);
            	return;
            }
        }
        
        EFaceDirection LeftDir, RightDir;
        switch (AwayDir)
        {
            case EFaceDirection::Xplus: LeftDir = EFaceDirection::Yminus; RightDir = EFaceDirection::Yplus; break;
            case EFaceDirection::Xminus: LeftDir = EFaceDirection::Yplus; RightDir = EFaceDirection::Yminus; break;
            case EFaceDirection::Yplus: LeftDir = EFaceDirection::Xminus; RightDir = EFaceDirection::Xplus; break;
            case EFaceDirection::Yminus: LeftDir = EFaceDirection::Xplus; RightDir = EFaceDirection::Xminus; break;
            default: LeftDir = RightDir = AwayDir; break;
        }
        
        EFaceDirection BestPerpDir = AwayDir;
        
        if(GetBestPerpendicularDirection(LeftDir, RightDir, GunterPos,BestPerpDir))
        {
            if(!bIsFleeing)
            {
                if(EnemyManager){EnemyManager->bIsGunterPlayingFearAnimation = true;}
                PlayFearAnimation();
            }
            StartFleeing(true, BestPerpDir);
            return;
        }
        
        int BestReachable = 0;
        float BestAvgDist = -1;
        
        TArray<EFaceDirection> PerpDirs = { LeftDir, RightDir };
        for (EFaceDirection PerpDir : PerpDirs)
        {
            FIntPoint TargetCoordPerp = GunterPos + GetDirectionDelta(PerpDir);
            
            if (TileManager->IsValidIndex(TargetCoordPerp.X, TargetCoordPerp.Y))
            {
                int TargetIndexPerp = TileManager->Get1DIndex(TargetCoordPerp.X, TargetCoordPerp.Y);
                
                if (CanMoveToTileIndex(TargetIndexPerp))
                {
                    TArray<int> ReachableIndices;
                    float AvgDistance = 0;
                    int ReachableCount = BFSCountReachableTilesInDirection(PerpDir, ReachableIndices, AvgDistance);
                    
                    if (ReachableCount > 0 && (ReachableCount > BestReachable || 
                        (ReachableCount == BestReachable && AvgDistance > BestAvgDist)))
                    {
                        BestReachable = ReachableCount;
                        BestAvgDist = AvgDistance;
                        BestPerpDir = PerpDir;
                    }
                }
            }
        }
        
        if (BestReachable > 0)
        {
            if(!bIsFleeing)
            {
                if(EnemyManager){EnemyManager->bIsGunterPlayingFearAnimation = true;}
                PlayFearAnimation();
            }
            StartFleeing(true, BestPerpDir);
            return;
        }
        
    }
}

bool AGoPawnEnemyGunter::CanMoveToTileIndex(int TileIndex) const
{
    if (!TileManager || !TileManager->Tiles.IsValidIndex(TileIndex)) return false;
    
    AGoTile* Tile = GetTileFromIndex(TileIndex);
    if (!Tile || !Tile->Walkable) return false;
    if (!TileManager->AreConnected(CurrTile->Index, TileIndex)) return false;
    
    if (EnemyManager)
    {
        AGoPawnEnemy* const* Occupant = EnemyManager->OccupiedTiles.Find(TileIndex);
        if (Occupant && *Occupant != this) {return false;}
    }
    
    if (PlayerRef && PlayerRef->CurrTile && PlayerRef->CurrTile->Index == TileIndex) { return false;}
    
    return true;
}

EFaceDirection AGoPawnEnemyGunter::GetBestDirectionCombined() const
{
    if (!TileManager || !CurrTile) return Direction;
    
    TArray<EFaceDirection> Directions = {
        EFaceDirection::Xplus,
        EFaceDirection::Xminus,
        EFaceDirection::Yplus,
        EFaceDirection::Yminus
    };
    
    struct FDirectionScore
    {
        EFaceDirection Dir;
        int ReachableTiles;
        float AvgDistanceFromPlayer;
        bool bHasImmediateMove;
    };
    
    TArray<FDirectionScore> Scores;
    FIntPoint GunterPos = TileManager->Get2DIndex(CurrTile->Index);
    
    for (EFaceDirection Dir : Directions)
    {
        FDirectionScore Score;
        Score.Dir = Dir;
        Score.ReachableTiles = 0;
        Score.AvgDistanceFromPlayer = 0;
        
        FIntPoint TargetCoord = GunterPos + GetDirectionDelta(Dir);
        if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
        {
            int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
            Score.bHasImmediateMove = CanMoveToTileIndex(TargetIndex);
            
            if (Score.bHasImmediateMove && PathfindingSubsystem && PlayerRef && PlayerRef->CurrTile)
            {
                TArray<int> ReachableIndices;
                Score.ReachableTiles = BFSCountReachableTilesInDirection(Dir, ReachableIndices, Score.AvgDistanceFromPlayer);
            }
        }
        else
        {
            Score.bHasImmediateMove = false;
        }
        
        Scores.Add(Score);
        
    }
    
    TArray<FDirectionScore> ValidScores;
    for (const FDirectionScore& Score : Scores)
    {
        if (Score.bHasImmediateMove && Score.ReachableTiles > 0)
        {
            ValidScores.Add(Score);
        }
    }
    
    if (ValidScores.Num() == 0) return Direction;
    
    ValidScores.Sort([](const FDirectionScore& A, const FDirectionScore& B) {
        if (A.ReachableTiles != B.ReachableTiles)
        {
            return A.ReachableTiles > B.ReachableTiles;
        }
        return A.AvgDistanceFromPlayer > B.AvgDistanceFromPlayer;
    });
    
    return ValidScores[0].Dir;
}

FMoveIntent AGoPawnEnemyGunter::ComputeMoveIntent_Implementation() const
{
    FMoveIntent Intent;
    Intent.NewDirection = Direction;
    Intent.TargetTile = CurrTile;

    if (!TileManager || !CurrTile || !bIsFleeing) {return Intent;}

    FIntPoint GunterCoord = TileManager->Get2DIndex(CurrTile->Index);
    FIntPoint TargetCoord = GunterCoord + GetDirectionDelta(FleeDirection);

    if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
    {
        int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
        AGoTile* TargetTilePtr = GetTileFromIndex(TargetIndex);

        if (TargetTilePtr && TargetTilePtr->Walkable && 
            TileManager->AreConnected(CurrTile->Index, TargetIndex))
        {
            if(EnemyManager->IsTileOccupied(TargetIndex, this) || TargetIndex == PlayerRef->CurrTile->Index)
                {const_cast<AGoPawnEnemyGunter*>(this)->bEnemyInFront = true; return Intent;}
            Intent.TargetTile = TargetTilePtr;
            Intent.NewDirection = FleeDirection;
            return Intent;
        }
    }

    const_cast<AGoPawnEnemyGunter*>(this)->bIsFleeing = false;
    return Intent;
}

void AGoPawnEnemyGunter::ApplyMoveIntent_Implementation(const FMoveIntent& Intent)
{
    Super::ApplyMoveIntent_Implementation(Intent);
}

void AGoPawnEnemyGunter::OnPostMove_Implementation()
{
    if (!bIsFleeing) return;
    
    if (CurrTile == PrevTile && !bEnemyInFront)
    {
        bIsFleeing = false;
    }
    bEnemyInFront = false;
    FIntPoint GunterCoord = TileManager->Get2DIndex(CurrTile->Index);
    FIntPoint TargetCoord = GunterCoord + GetDirectionDelta(FleeDirection);

    if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
    {
        int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
        AGoTile* TargetTilePtr = GetTileFromIndex(TargetIndex);

        if (!TargetTilePtr || !TargetTilePtr->Walkable || 
            !TileManager->AreConnected(CurrTile->Index, TargetIndex))
        {
            bIsFleeing = false;
        }
    }
    else 
    {
        bIsFleeing = false;
    }
    
    if(!bIsFleeing)
    {
        EFaceDirection LeftDir, RightDir;
        switch (Direction)
        {
            case EFaceDirection::Xplus: LeftDir = EFaceDirection::Yminus; RightDir = EFaceDirection::Yplus; break;
            case EFaceDirection::Xminus: LeftDir = EFaceDirection::Yplus; RightDir = EFaceDirection::Yminus; break;
            case EFaceDirection::Yplus: LeftDir = EFaceDirection::Xminus; RightDir = EFaceDirection::Xplus; break;
            case EFaceDirection::Yminus: LeftDir = EFaceDirection::Xplus; RightDir = EFaceDirection::Xminus; break;
            default: LeftDir = RightDir = Direction; break;
        }
        
        TArray<int> LeftReachable;
        TArray<int> RightReachable;
        float LeftAvgDist = 0;
        float RightAvgDist = 0;
        bool bLeftHasPlayer = false;
        bool bRightHasPlayer = false;
        
        int LeftCount = BFSCountReachableTilesInDirection(LeftDir, LeftReachable, LeftAvgDist);
        int RightCount = BFSCountReachableTilesInDirection(RightDir, RightReachable, RightAvgDist);
        
        bool bChooseLeft;
        if (LeftCount > 0 || RightCount > 0)
        {
            if (bLeftHasPlayer != bRightHasPlayer)
            {
                bChooseLeft = !bLeftHasPlayer;
            }
            else if (LeftCount != RightCount)
            {
                bChooseLeft = LeftCount > RightCount;
            }
            else
            {
                bChooseLeft = LeftAvgDist > RightAvgDist;
            }
            EFaceDirection ChosenDir = bChooseLeft ? LeftDir : RightDir;
            PendingMoveTile = nullptr;
            StartRotationToDirection(ChosenDir, RotationDuration);
        }
        else
        {
            EFaceDirection NewFaceDirection = GetBestDirectionCombined();
            PendingMoveTile = nullptr;
            StartRotationToDirection(NewFaceDirection, RotationDuration);
        }
    }
    
    PrevTile = CurrTile;
}

void AGoPawnEnemyGunter::ApplyDamage_Implementation(int Amount, EFaceDirection HitDirection)
{
    bWasHitThisTurn = true;
    Health -= Amount;

    OnDamageTaken(Amount, HitDirection);
    
    if (Health <= 0)
    {
        OnDeath();
        OnEnemyDeath.Broadcast(this);
        return;
    }

    QueuedMoveDirection = GetBestDirectionCombined();
    bQueuedMoveFromHit = true;

    if(EnemyManager){EnemyManager->bIsGunterPlayingFearAnimation = true;}
    PlayFearAnimation();
}

void AGoPawnEnemyGunter::StartFleeing(bool bSetDirection, EFaceDirection AwayDir)
{
    bIsFleeing = true;
    
    if (bSetDirection)
    {
        FleeDirection = AwayDir;
    }
}

void AGoPawnEnemyGunter::OnFearAnimationComplete()
{
    if(EnemyManager){EnemyManager->OnGunterFearAnimationComplete();}
}

void AGoPawnEnemyGunter::ExecuteQueuedMove()
{
    if (!bQueuedMoveFromHit) return;
    
    bQueuedMoveFromHit = false;
    StartFleeing(true, QueuedMoveDirection);
    
    FMoveIntent Intent;
    FIntPoint GunterCoord = TileManager->Get2DIndex(CurrTile->Index);
    FIntPoint TargetCoord = GunterCoord + GetDirectionDelta(QueuedMoveDirection);
    
    if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
    {
        int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
        Intent.TargetTile = GetTileFromIndex(TargetIndex);
        Intent.NewDirection = QueuedMoveDirection;
        
        if (Intent.TargetTile && Intent.TargetTile != CurrTile)
        {
            ApplyMoveIntent_Implementation(Intent);
        }
    }
}

void AGoPawnEnemyGunter::OnExecutedQueuedMovedComplete()
{
    if (EnemyManager) {EnemyManager->OnGunterImmediateMoveCompleted();}
}

void AGoPawnEnemyGunter::PlayFearAnimation_Implementation()
{
    //Blueprint implementation
}

int AGoPawnEnemyGunter::BFSCountReachableTilesInDirection(EFaceDirection Dir, TArray<int>& OutReachableTiles, float& OutAvgDistance) const
{
    if (!PathfindingSubsystem || !PlayerRef || !PlayerRef->CurrTile) 
    {
        return 0;
    }
    
    return PathfindingSubsystem->CountReachableTilesInDirection(
        CurrTile->Index, Dir, PlayerRef->CurrTile->Index, EnemyManager, OutReachableTiles, OutAvgDistance);
}

int AGoPawnEnemyGunter::CountAvailableTilesInDirection(EFaceDirection Dir, const FIntPoint& StartPos) const
{
    if (!TileManager) return 0;
    int Count = 0;
    FIntPoint CurrentPos = StartPos;
    int LastValidIndex = TileManager->Get1DIndex(StartPos.X, StartPos.Y);
    
    while (true)
    {
        FIntPoint NextPos = CurrentPos + GetDirectionDelta(Dir);
        if (!TileManager->IsValidIndex(NextPos.X, NextPos.Y)){break;}
        int NextIndex = TileManager->Get1DIndex(NextPos.X, NextPos.Y);
        AGoTile* NextTile = GetTileFromIndex(NextIndex);
        if (!NextTile || !NextTile->Walkable){break;}
        if (!TileManager->AreConnected(LastValidIndex, NextIndex)){break;}
        if (EnemyManager && EnemyManager->IsTileOccupied(NextIndex, this)){break;}
        if (PlayerRef && PlayerRef->CurrTile && PlayerRef->CurrTile->Index == NextIndex){break;}
        
        Count++;
        CurrentPos = NextPos;
        LastValidIndex = NextIndex;
    }
    
    return Count;
}

bool AGoPawnEnemyGunter::GetBestPerpendicularDirection(EFaceDirection LeftDir, EFaceDirection RightDir, const FIntPoint& GunterPos, EFaceDirection& OutBestDir) const
{
    int LeftCount = CountAvailableTilesInDirection(LeftDir, GunterPos);
    int RightCount = CountAvailableTilesInDirection(RightDir, GunterPos);
    if (LeftCount > 0 || RightCount > 0){
        if (LeftCount > RightCount) {OutBestDir = LeftDir; return true;}
        if (RightCount > LeftCount) {OutBestDir = RightDir; return true;}
        if (LeftCount == RightCount) {return false;}
    }
    return false;
}