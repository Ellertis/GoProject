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
    
    PreferredFleeOrder = {
        EFaceDirection::Xplus,
        EFaceDirection::Yplus,
        EFaceDirection::Xminus,
        EFaceDirection::Yminus
    };
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

    if (bHasPendingFlee)
    {
        StartFleeing(true, PendingFleeDirection);
        bHasPendingFlee = false;
    }
	
    FIntPoint GunterPos = TileManager->Get2DIndex(CurrTile->Index);
    FIntPoint PlayerPos = TileManager->Get2DIndex(PlayerRef->CurrTile->Index);
    int ManhattanDist = FMath::Abs(GunterPos.X - PlayerPos.X) + FMath::Abs(GunterPos.Y - PlayerPos.Y);
	
    if (ManhattanDist == 1)
    {
    	if (!TileManager->AreConnected(CurrTile->Index, PlayerRef->CurrTile->Index))
    	{
    		return; // Tile between player and Gunter are not connected, no fleeing
    	}
        
        FIntPoint Delta = GunterPos - PlayerPos;
        EFaceDirection AwayDir = GetDirectionFromDelta(Delta);
        
        // Try to flee directly away from player
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
        
        // If away direction is blocked, try perpendicular directions to AwayDir
        EFaceDirection LeftDir, RightDir;
        switch (AwayDir)
        {
            case EFaceDirection::Xplus: LeftDir = EFaceDirection::Yminus;RightDir = EFaceDirection::Yplus;break;
            case EFaceDirection::Xminus: LeftDir = EFaceDirection::Yplus;RightDir = EFaceDirection::Yminus;break;
            case EFaceDirection::Yplus: LeftDir = EFaceDirection::Xminus;RightDir = EFaceDirection::Xplus;break;
            case EFaceDirection::Yminus: LeftDir = EFaceDirection::Xplus;RightDir = EFaceDirection::Xminus;break;
            default: LeftDir = RightDir = AwayDir;break;
        }
        
        // Pathfinding
        if (PathfindingSubsystem && PlayerRef)
        {
            TArray<int> LeftPath = PathfindingSubsystem->FindDirectionalFleePath(
                CurrTile->Index, LeftDir, PlayerRef->CurrTile->Index);
            TArray<int> RightPath = PathfindingSubsystem->FindDirectionalFleePath(
                CurrTile->Index, RightDir, PlayerRef->CurrTile->Index);
            int LeftPathLength = LeftPath.Num();
            int RightPathLength = RightPath.Num();

            if (LeftPathLength > 0 || RightPathLength > 0)
            {
                EFaceDirection ChosenDir = (LeftPathLength >= RightPathLength) ? LeftDir : RightDir;
                if(!bIsFleeing)
                {
                    if(EnemyManager){EnemyManager->bIsGunterPlayingFearAnimation = true;}
                    PlayFearAnimation();
                }
                StartFleeing(true, ChosenDir);
                return;
            }
        }
        
        // Fallback to perpendicular tile counting
        int LeftPathLength = 0;
        int RightPathLength = 0;
        
        FIntPoint LeftCoord = GunterPos + GetDirectionDelta(LeftDir);
        if (TileManager->IsValidIndex(LeftCoord.X, LeftCoord.Y))
        {
            int LeftIndex = TileManager->Get1DIndex(LeftCoord.X, LeftCoord.Y);
            if (CanMoveToTileIndex(LeftIndex)) {LeftPathLength = CountWalkableTilesInDirection(GunterPos, LeftDir, -1);}
        }
        
        FIntPoint RightCoord = GunterPos + GetDirectionDelta(RightDir);
        if (TileManager->IsValidIndex(RightCoord.X, RightCoord.Y))
        {
            int RightIndex = TileManager->Get1DIndex(RightCoord.X, RightCoord.Y);
            if (CanMoveToTileIndex(RightIndex)) {RightPathLength = CountWalkableTilesInDirection(GunterPos, RightDir, -1);}
        }
        
        if (LeftPathLength > 0 || RightPathLength > 0)
        {
            EFaceDirection ChosenDir = (LeftPathLength >= RightPathLength) ? LeftDir : RightDir;
            if(!bIsFleeing)
            {
                if(EnemyManager){EnemyManager->bIsGunterPlayingFearAnimation = true;}
                PlayFearAnimation();
            }
            StartFleeing(true, ChosenDir);
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

EFaceDirection AGoPawnEnemyGunter::GetBestFleeDirection() const
{
    if (!TileManager || !CurrTile) return Direction;

    FIntPoint GunterPos = TileManager->Get2DIndex(CurrTile->Index);
    TArray<EFaceDirection> ValidDirections;
    TArray<int> PathLengths;

    // Check all four directions
    TArray<EFaceDirection> AllDirs = {
        EFaceDirection::Xplus,
        EFaceDirection::Xminus,
        EFaceDirection::Yplus,
        EFaceDirection::Yminus
    };

    for (EFaceDirection Dir : AllDirs)
    {
        FIntPoint TargetCoord = GunterPos + GetDirectionDelta(Dir);

        if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
        {
            int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
            if (CanMoveToTileIndex(TargetIndex))
            {
                int PathLength = CountWalkableTilesInDirection(GunterPos, Dir, -1);
                ValidDirections.Add(Dir);
                PathLengths.Add(PathLength);
            }
        }
    }

    if (ValidDirections.Num() == 0) return Direction;
    int BestIndex = 0;
    int BestLength = PathLengths[0];

    for (int i = 1; i < ValidDirections.Num(); i++)
    {
        if (PathLengths[i] > BestLength)
        {
            BestLength = PathLengths[i];
            BestIndex = i;
        }
    }

    return ValidDirections[BestIndex];
}

FMoveIntent AGoPawnEnemyGunter::ComputeMoveIntent_Implementation() const
{
    FMoveIntent Intent;
    Intent.NewDirection = Direction;
    Intent.TargetTile = CurrTile;

    if (!TileManager || !CurrTile || !bIsFleeing) {return Intent;}

    FIntPoint GunterCoord = TileManager->Get2DIndex(CurrTile->Index);
    FIntPoint TargetCoord = GunterCoord + GetDirectionDelta(FleeDirection);

    // Check if Gunter can move forward
    if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
    {
        int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
        AGoTile* TargetTilePtr = GetTileFromIndex(TargetIndex);

        if (TargetTilePtr && TargetTilePtr->Walkable && 
            TileManager->AreConnected(CurrTile->Index, TargetIndex) &&
            !EnemyManager->IsTileOccupied(TargetIndex, this))
        {
            Intent.TargetTile = TargetTilePtr;
            Intent.NewDirection = FleeDirection;
            return Intent;
        }
    }

    // Forward is blocked, stop fleeing
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
    
    if (bIsFleeing && CurrTile == PrevTile)
    {
        bIsFleeing = false;
    }
    
	FIntPoint GunterCoord = TileManager->Get2DIndex(CurrTile->Index);
	FIntPoint TargetCoord = GunterCoord + GetDirectionDelta(FleeDirection);

	// Check if Gunter can move forward
	if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
	{
		int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
		AGoTile* TargetTilePtr = GetTileFromIndex(TargetIndex);

		if (!TargetTilePtr || !TargetTilePtr->Walkable || 
			!TileManager->AreConnected(CurrTile->Index, TargetIndex) ||
			EnemyManager->IsTileOccupied(TargetIndex, this))
		{
			bIsFleeing = false;
		}
	}
    else {bIsFleeing = false;}
    
	// Forward is blocked, stop fleeing
    
    /* WIP 
    // Guessing the best pathfinding flee direction 
    EFaceDirection ResultDir = Direction;
    if (PathfindingSubsystem && PlayerRef)
    {
        TArray<int> XplusPath = PathfindingSubsystem->FindDirectionalFleePath(
            CurrTile->Index, EFaceDirection::Xplus, PlayerRef->CurrTile->Index);
        TArray<int> XminusPath = PathfindingSubsystem->FindDirectionalFleePath(
            CurrTile->Index, EFaceDirection::Xminus, PlayerRef->CurrTile->Index);
        TArray<int> YplusPath = PathfindingSubsystem->FindDirectionalFleePath(
            CurrTile->Index, EFaceDirection::Yplus, PlayerRef->CurrTile->Index);
        TArray<int> YminusPath = PathfindingSubsystem->FindDirectionalFleePath(
            CurrTile->Index, EFaceDirection::Yminus, PlayerRef->CurrTile->Index);
        TArray<int> PathLengths = {XplusPath.Num(), YplusPath.Num(), XminusPath.Num(), YminusPath.Num()};
        int LongestPathLength = FMath::Max(PathLengths);
        TArray<EFaceDirection> Directions = {EFaceDirection::Xplus, EFaceDirection::Xminus, EFaceDirection::Yplus, EFaceDirection::Yminus};
        for(int i = 0; i < 3; i++){if(LongestPathLength == PathLengths[i]) ResultDir = Directions[i];}
    }
    switch (ResultDir)
    */

    if(!bIsFleeing)
    {
        FRotator NewRotation = FRotator::ZeroRotator;
        switch (GetBestFleeDirection())
        {
            case EFaceDirection::Xplus:  NewRotation = FRotator(0, 0, 0); break;
            case EFaceDirection::Xminus: NewRotation = FRotator(0, 180, 0); break;
            case EFaceDirection::Yplus:  NewRotation = FRotator(0, 90, 0); break;
            case EFaceDirection::Yminus: NewRotation = FRotator(0, -90, 0); break;
        }
        SetActorRotation(NewRotation);
    }
    
    PrevTile = CurrTile;
}

void AGoPawnEnemyGunter::ApplyDamage_Implementation(int Amount, EFaceDirection HitDirection)
{
    Health -= Amount;
    
    if (Health <= 0)
    {
        OnDeath();
        OnEnemyDeath.Broadcast(this);
        //Destroy(); should be implemented in bp
        return;
    }
	
    EFaceDirection PerpDir1, PerpDir2;
    switch (HitDirection)
    {
        case EFaceDirection::Xplus:
        case EFaceDirection::Xminus:
            PerpDir1 = EFaceDirection::Yplus;
            PerpDir2 = EFaceDirection::Yminus;
            break;
        case EFaceDirection::Yplus:
        case EFaceDirection::Yminus:
            PerpDir1 = EFaceDirection::Xplus;
            PerpDir2 = EFaceDirection::Xminus;
            break;
        default:
            PerpDir1 = EFaceDirection::Xplus;
            PerpDir2 = EFaceDirection::Xminus;
            break;
    }
	
    EFaceDirection FleeDir;

	//Pathfind route to escape from snowballs and the player
    if (PathfindingSubsystem && PlayerRef && PlayerRef->CurrTile)
    {
        TArray<int> Path1 = PathfindingSubsystem->FindDirectionalFleePath(
            CurrTile->Index, PerpDir1, PlayerRef->CurrTile->Index);
        TArray<int> Path2 = PathfindingSubsystem->FindDirectionalFleePath(
            CurrTile->Index, PerpDir2, PlayerRef->CurrTile->Index);
        
        FleeDir = (Path1.Num() >= Path2.Num()) ? PerpDir1 : PerpDir2;
    }
    else
    {
        // Fallback to direction choice
        FIntPoint GunterPos = TileManager->Get2DIndex(CurrTile->Index);
        int LeftCount = CountWalkableTilesInDirection(GunterPos, PerpDir1, PlayerRef->CurrTile->Index);
        int RightCount = CountWalkableTilesInDirection(GunterPos, PerpDir2, PlayerRef->CurrTile->Index);
        
        FleeDir = (LeftCount >= RightCount) ? PerpDir1 : PerpDir2;
    }

    PendingFleeDirection = FleeDir;
    bHasPendingFlee = true;
}

void AGoPawnEnemyGunter::StartFleeing(bool bSetDirection, EFaceDirection AwayDir)
{
    bIsFleeing = true;
    
    if (bSetDirection)
    {
        Direction = AwayDir;
        FleeDirection = AwayDir;
    }
}

void AGoPawnEnemyGunter::OnFearAnimationComplete()
{
    if(EnemyManager){EnemyManager->OnGunterFearAnimationComplete();}
}

void AGoPawnEnemyGunter::PlayFearAnimation_Implementation()
{
    //Blueprint implementation
}

int AGoPawnEnemyGunter::CountWalkableTilesInDirection(const FIntPoint& StartCoord, EFaceDirection Dir, int IgnoreTileIndex) const
{
    int Count = 0;
    FIntPoint Current = StartCoord;
    FIntPoint DirVec = GetDirectionDelta(Dir);
    int CurrentIndex = TileManager->Get1DIndex(Current.X, Current.Y);
    
    while (true)
    {
        FIntPoint Next = Current + DirVec;
        if (!TileManager->IsValidIndex(Next.X, Next.Y)) {break;}
        
        int NextIndex = TileManager->Get1DIndex(Next.X, Next.Y);
        AGoTile* Tile = GetTileFromIndex(NextIndex);
    	
        if (!Tile || !Tile->Walkable || !TileManager->AreConnected(CurrentIndex, NextIndex)
        	|| NextIndex == IgnoreTileIndex || EnemyManager && EnemyManager->IsTileOccupied(NextIndex, this))
        {
            break;
        }
    	
        Count++;
        Current = Next;
        CurrentIndex = NextIndex;
    }
	
    return Count;
}
