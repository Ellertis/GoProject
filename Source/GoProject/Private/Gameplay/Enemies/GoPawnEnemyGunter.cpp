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
        
        // First, try to flee directly away from player
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
        
        // If away direction is blocked, evaluate perpendicular directions
        EFaceDirection LeftDir, RightDir;
        switch (AwayDir)
        {
            case EFaceDirection::Xplus: LeftDir = EFaceDirection::Yminus; RightDir = EFaceDirection::Yplus; break;
            case EFaceDirection::Xminus: LeftDir = EFaceDirection::Yplus; RightDir = EFaceDirection::Yminus; break;
            case EFaceDirection::Yplus: LeftDir = EFaceDirection::Xminus; RightDir = EFaceDirection::Xplus; break;
            case EFaceDirection::Yminus: LeftDir = EFaceDirection::Xplus; RightDir = EFaceDirection::Xminus; break;
            default: LeftDir = RightDir = AwayDir; break;
        }
        
        // Evaluate both perpendicular directions
        EFaceDirection BestPerpDir = AwayDir;
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
                    int ReachableCount = CountReachableTilesInDirection(PerpDir, ReachableIndices, AvgDistance);
                    
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
        if(bIsFleeing){bIsFleeing = false;}
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
                Score.ReachableTiles = CountReachableTilesInDirection(Dir, ReachableIndices, Score.AvgDistanceFromPlayer);
            }
        }
        else
        {
            Score.bHasImmediateMove = false;
        }
        
        Scores.Add(Score);
        
    }
    
    // Filter to directions with at least one move
    TArray<FDirectionScore> ValidScores;
    for (const FDirectionScore& Score : Scores)
    {
        if (Score.bHasImmediateMove && Score.ReachableTiles > 0)
        {
            ValidScores.Add(Score);
        }
    }
    
    if (ValidScores.Num() == 0) return Direction;
    
    // Sort by reachable tiles (descending), then by average distance from player (descending)
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
            TileManager->AreConnected(CurrTile->Index, TargetIndex)) //&&
            //!EnemyManager->IsTileOccupied(TargetIndex, this))
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
            !TileManager->AreConnected(CurrTile->Index, TargetIndex)) //||
            //EnemyManager->IsTileOccupied(TargetIndex, this))
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
        EFaceDirection NewFaceDirection = GetBestDirectionCombined();
        PendingMoveTile = nullptr; //crucial to not trigger movement logic after rotation is finished
        StartRotationToDirection(NewFaceDirection, RotationDuration);
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

int AGoPawnEnemyGunter::CountReachableTilesInDirection(EFaceDirection Dir, TArray<int>& OutReachableTiles, float& OutAvgDistance) const
{
    if (!PathfindingSubsystem || !PlayerRef || !PlayerRef->CurrTile) 
    {
        return 0;
    }
    
    return PathfindingSubsystem->CountReachableTilesInDirection(
        CurrTile->Index, Dir, PlayerRef->CurrTile->Index, EnemyManager, OutReachableTiles, OutAvgDistance);
}