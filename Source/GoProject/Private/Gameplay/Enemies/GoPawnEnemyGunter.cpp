// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoPawnEnemyGunter.h"

#include "Core/GoGameModeBase.h"

AGoPawnEnemyGunter::AGoPawnEnemyGunter()
{
	Health = StartHealth;
}

void AGoPawnEnemyGunter::PreTurnUpdate_Implementation()
{
	if (!TileManager || !CurrTile || !PlayerRef || !PlayerRef->CurrTile)
		return;

	// Check if player is adjacent
	FIntPoint GunterPos = TileManager->Get2DIndex(CurrTile->Index);
	FIntPoint PlayerPos = TileManager->Get2DIndex(PlayerRef->CurrTile->Index);
	if (FMath::Abs(GunterPos.X - PlayerPos.X) + FMath::Abs(GunterPos.Y - PlayerPos.Y) == 1)
	{
		FIntPoint Delta = GunterPos - PlayerPos;
		EFaceDirection AwayDir = GetDirectionFromDelta(Delta);
        FIntPoint TargetCoord = GunterPos + GetDirectionDelta(AwayDir);

        if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
        {
            int TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
            AGoTile* TargetTile = TileManager->Tiles[TargetIndex];
            if(TargetTile && TargetTile->Walkable){StartFleeing(true, AwayDir);return;}
        }
		
		// Away direction blocked – choose perpendicular
		EFaceDirection LeftDir, RightDir;
		switch (AwayDir)
		{
			case EFaceDirection::Xplus:  LeftDir = EFaceDirection::Yminus; RightDir = EFaceDirection::Yplus; break;
			case EFaceDirection::Xminus: LeftDir = EFaceDirection::Yplus;  RightDir = EFaceDirection::Yminus; break;
			case EFaceDirection::Yplus:  LeftDir = EFaceDirection::Xminus; RightDir = EFaceDirection::Xplus;  break;
			case EFaceDirection::Yminus: LeftDir = EFaceDirection::Xplus;  RightDir = EFaceDirection::Xminus; break;
			default: LeftDir = RightDir = AwayDir;
		}

		int LeftCount = CountWalkableTilesInDirection(GunterPos, LeftDir, PlayerRef->CurrTile->Index);
		int RightCount = CountWalkableTilesInDirection(GunterPos, RightDir, PlayerRef->CurrTile->Index);
		if (LeftCount > 0 || RightCount > 0)
		{
			EFaceDirection ChosenDir = (LeftCount >= RightCount) ? LeftDir : RightDir;
			StartFleeing(true, ChosenDir);
			return;
		}
		//TODO// No perpendicular path either // Push the player back
	}
}

FEnemyMoveIntent AGoPawnEnemyGunter::ComputeMoveIntent_Implementation() const
{
	FEnemyMoveIntent Intent;
	Intent.NewDirection = Direction;
	Intent.TargetTile = CurrTile;

	if (!TileManager || !CurrTile || !PlayerRef || !bIsFleeing) return Intent;

	FIntPoint GunterCoord = TileManager->Get2DIndex(CurrTile->Index);
	int32 PlayerTileIndex = PlayerRef->CurrTile->Index;
	
	FIntPoint TargetCoord = GunterCoord + GetDirectionDelta(Direction);
	if (TileManager->IsValidIndex(TargetCoord.X, TargetCoord.Y))
	{
		int32 TargetIndex = TileManager->Get1DIndex(TargetCoord.X, TargetCoord.Y);
		AGoTile* TargetTile = TileManager->Tiles[TargetIndex];
		if (TargetTile && TargetTile->Walkable && TargetIndex != PlayerTileIndex)
		{
			Intent.TargetTile = TargetTile;
			return Intent;
		}
	}
	.
	return Intent;
}

void AGoPawnEnemyGunter::ApplyMoveIntent_Implementation(const FEnemyMoveIntent& Intent)
{
	PrevTile = CurrTile;
	Super::ApplyMoveIntent_Implementation(Intent);
}

void AGoPawnEnemyGunter::OnPostMove_Implementation()
{
	if (bIsFleeing && CurrTile == PrevTile)
	{
		bIsFleeing = false;
	}
}

void AGoPawnEnemyGunter::ApplyDamage_Implementation(int Amount)
{
	Health -= Amount;
	if (Health <= 0)
	{
		OnEnemyDeath.Broadcast(this);
		Destroy();
	}
	StartFleeing(false, Direction);
}


void AGoPawnEnemyGunter::StartFleeing(bool bSetDirection, EFaceDirection AwayDir)
{
	bIsFleeing = true;
	if(bSetDirection) Direction = AwayDir;
}

int AGoPawnEnemyGunter::CountWalkableTilesInDirection(const FIntPoint& StartCoord, EFaceDirection Dir, int IgnoreTileIndex) const
{
	int Count = 0;
	FIntPoint Current = StartCoord;
	while (true)
	{
		FIntPoint Next = Current + GetDirectionDelta(Dir);
		if (!TileManager->IsValidIndex(Next.X, Next.Y)) break;
		int NextIndex = TileManager->Get1DIndex(Next.X, Next.Y);
		AGoTile* Tile = TileManager->Tiles[NextIndex];
		if (!Tile || !Tile->Walkable || NextIndex == IgnoreTileIndex) break;
		Count++;
		Current = Next;
	}
	return Count;
}

void AGoPawnEnemyGunter::BeginPlay()
{
	Super::BeginPlay();
	const AGoGameModeBase* GameModeBase = (AGoGameModeBase*)GetWorld()->GetAuthGameMode();
	PlayerRef = GameModeBase->GetPlayer();
	if (!PlayerRef) UE_LOG(LogTemp, Warning, TEXT("Player could not be found"));
}
