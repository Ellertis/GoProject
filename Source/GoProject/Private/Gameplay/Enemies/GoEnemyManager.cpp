// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Gameplay/Enemies/GoPawnEnemyGunter.h"
#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Enemies/GoSnowball.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "Core/GoPathfindingSubsystem.h"
#include "Player/GoPawnPlayer.h"
#include "Core/GoGameModeBase.h"
#include "Core/GoPawn.h"
#include "Gameplay/Enemies/GoPawnEnemy.h"

AGoEnemyManager::AGoEnemyManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bWaitingToEndTurn = false;
    bIsProcessingTurn = false;
}

void AGoEnemyManager::BeginPlay()
{
    Super::BeginPlay();
    PathfindingSubsystem = GetWorld()->GetSubsystem<UGoPathfindingSubsystem>();
}

void AGoEnemyManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGoEnemyManager::OnGunterFearAnimationComplete()
{
	bIsGunterPlayingFearAnimation = false;
	
	TArray<AGoPawnEnemySnowmen*> Snowmen;
	for (AGoPawnEnemy* Enemy : Enemies)
	{
		if (AGoPawnEnemySnowmen* Snowman = Cast<AGoPawnEnemySnowmen>(Enemy))
		{
			Snowmen.Add(Snowman);
		}
	}
	
	GunterPhase(CurrentGunter, Snowmen);
}

void AGoEnemyManager::OnGunterImmediateMoveCompleted()
{
	UpdateOccupancy();
	
	for (AGoPawnEnemy* Enemy : Enemies)
	{
		if (AGoPawnEnemyGunter* Gunter = Cast<AGoPawnEnemyGunter>(Enemy)) {Gunter->bIsFleeing = true; Gunter->OnPostMove();}
	}
}

void AGoEnemyManager::OnEnemyMoveCompleted(AGoPawnEnemy* Enemy)
{
	if (!Enemy || bIsEndingTurn) return;
    
    UpdateOccupancy();
    
	if (Enemy->IsA(AGoPawnEnemyGunter::StaticClass()))
	{
		PendingGunterMoves--;
		
		if (PendingGunterMoves == 0)
		{
			TArray<AGoPawnEnemySnowmen*> Snowmen;
			for (AGoPawnEnemy* E : Enemies)
			{
				if (AGoPawnEnemySnowmen* S = Cast<AGoPawnEnemySnowmen>(E))
				{
					Snowmen.Add(S);
				}
			}
			SnowmanPhase(Snowmen);
		}
	}
	else if (Enemy->IsA(AGoPawnEnemySnowmen::StaticClass()))
	{
		PendingSnowmanMoves--;
		if (PendingSnowmanMoves <= 0) {FinishEnemyTurn();}
	}
}

void AGoEnemyManager::OnNewEnemyTurn(const ETurnPhase NewTurnPhase)
{
    if (NewTurnPhase != ETurnPhase::EnemyTurn) return;
	if (PlayerRef) {PlayerRef->CheckJakeTileRemoval();}
    bIsProcessingTurn = true;
	bIsEndingTurn = false;
    bWaitingToEndTurn = false;
	bIsGunterPlayingFearAnimation = false;
	CurrentGunter = nullptr;
	PendingGunterMoves = 0;
	PendingSnowmanMoves = 0;
	
    TArray<AGoPawnEnemySnowmen*> Snowmen;
    AGoPawnEnemyGunter* Gunter = nullptr;

    for (AGoPawnEnemy* Enemy : Enemies)
    {
        if (AGoPawnEnemySnowmen* Snowman = Cast<AGoPawnEnemySnowmen>(Enemy)) {Snowmen.Add(Snowman);}
        else if (AGoPawnEnemyGunter* G = Cast<AGoPawnEnemyGunter>(Enemy)) {Gunter = G;}
    }

	CurrentGunter = Gunter;
	
    for (AGoPawnEnemy* Enemy : Enemies) {Enemy->PreTurnUpdate();}

    UpdateOccupancy();

	if(bIsGunterPlayingFearAnimation)
	{
		return;
	}
	
	GunterPhase(Gunter,Snowmen);
}

void AGoEnemyManager::SnowmanPhase(TArray<AGoPawnEnemySnowmen*>& Snowmen)
{
	PendingSnowmanMoves = 0;
	for (AGoPawnEnemySnowmen* Snowman : Snowmen)
	{
		FMoveIntent SnowmanIntent = Snowman->ComputeMoveIntent();
		if (SnowmanIntent.TargetTile != Snowman->CurrTile)
		{
			OccupiedTiles.Add(SnowmanIntent.TargetTile->Index, Snowman);
			PendingSnowmanMoves++;
			Snowman->ApplyMoveIntent(SnowmanIntent);
		}
	}
    
	UpdateOccupancy();
	
	if (PendingSnowmanMoves == 0){FinishEnemyTurn();}
}

void AGoEnemyManager::GunterPhase(AGoPawnEnemyGunter* Gunter, TArray<AGoPawnEnemySnowmen*>& Snowmen)
{
	if (Gunter)
	{
		FMoveIntent GunterIntent = Gunter->ComputeMoveIntent();
		if (GunterIntent.TargetTile != Gunter->CurrTile)
		{
			PendingGunterMoves++;
			Gunter->ApplyMoveIntent(GunterIntent);
		}
	}
    
	UpdateOccupancy();

	if (PendingGunterMoves == 0){SnowmanPhase(Snowmen);}
}

void AGoEnemyManager::CheckSnowmanAttacks()
{
    TArray<AGoPawnEnemySnowmen*> Snowmen;
    for (AGoPawnEnemy* Enemy : Enemies)
    {
        if (AGoPawnEnemySnowmen* Snowman = Cast<AGoPawnEnemySnowmen>(Enemy))
            Snowmen.Add(Snowman);
    }
    
    if (Snowmen.Num() < 2) {return;}
	
    // Group by X and Y coordinates
    TMap<int, TArray<AGoPawnEnemySnowmen*>> ByX;
    TMap<int, TArray<AGoPawnEnemySnowmen*>> ByY;
    
    for (AGoPawnEnemySnowmen* S : Snowmen)
    {
        if (!S->CurrTile) continue;
        FIntPoint Pos = TileManager->Get2DIndex(S->CurrTile->Index);
        ByX.FindOrAdd(Pos.X).Add(S);
        ByY.FindOrAdd(Pos.Y).Add(S);
    }

    // Check vertical alignments X coordinate
    for (TPair<int, TArray<AGoPawnEnemySnowmen*>>& Pair : ByX)
    {
	    TArray<AGoPawnEnemySnowmen*>& Group = Pair.Value;
    	if (Group.Num() < 2) continue;
    	
    	Group.Sort([this](AGoPawnEnemySnowmen& A, AGoPawnEnemySnowmen& B) {
			FIntPoint PosA = TileManager->Get2DIndex(A.CurrTile->Index);
			FIntPoint PosB = TileManager->Get2DIndex(B.CurrTile->Index);
			return PosA.Y < PosB.Y;
		});
    	
    	for (int i = 0; i < Group.Num(); i++)
    	{
    		for (int j = i + 1; j < Group.Num(); j++)
    		{
    			AGoPawnEnemySnowmen* S1 = Group[i];
    			AGoPawnEnemySnowmen* S2 = Group[j];
                
    			FVector Start = S1->GetActorLocation()+FVector(0, 0, 50);
    			FVector End = S2->GetActorLocation()+FVector(0, 0, 50);;
            	
    			OnSnowmanAttack(Start, End, EFaceDirection::Yplus);
            	
    			if (SnowballClass)
    			{
    				AGoSnowball* Snowball1 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, Start, FRotator::ZeroRotator);
    				AGoSnowball* Snowball2 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, End, FRotator::ZeroRotator);
                    
    				if (Snowball1 && Snowball2)
    				{
    					Snowball1->EnemyManager = this;
    					Snowball1->ProjectedDirection = EFaceDirection::Yplus;
    					Snowball1->Launch(Start, End);
                        
    					Snowball2->EnemyManager = this;
    					Snowball2->ProjectedDirection = EFaceDirection::Yminus;
    					Snowball2->Launch(End, Start);
    				}
    			}
    		}
    	}
    }
	
    // Check horizontal alignments Y coordinate
    for (TPair<int, TArray<AGoPawnEnemySnowmen*>>& Pair : ByY)
    {
        TArray<AGoPawnEnemySnowmen*>& Group = Pair.Value;
        if (Group.Num() < 2) continue;
    	
        Group.Sort([this](AGoPawnEnemySnowmen& A, AGoPawnEnemySnowmen& B) {
            FIntPoint PosA = TileManager->Get2DIndex(A.CurrTile->Index);
            FIntPoint PosB = TileManager->Get2DIndex(B.CurrTile->Index);
            return PosA.X < PosB.X;
        });
    	
        for (int i = 0; i < Group.Num(); i++)
        {
            for (int j = i + 1; j < Group.Num(); j++)
            {
                AGoPawnEnemySnowmen* S1 = Group[i];
                AGoPawnEnemySnowmen* S2 = Group[j];
                
                FVector Start = S1->GetActorLocation()+FVector(0, 0, 50);;
                FVector End = S2->GetActorLocation()+FVector(0, 0, 50);;

                OnSnowmanAttack(Start, End, EFaceDirection::Xplus);

                if (SnowballClass)
                {
                    AGoSnowball* Snowball1 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, Start, FRotator::ZeroRotator);
                    AGoSnowball* Snowball2 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, End, FRotator::ZeroRotator);
                    
                    if (Snowball1 && Snowball2)
                    {
                        Snowball1->EnemyManager = this;
                        Snowball1->ProjectedDirection = EFaceDirection::Xplus;
                        Snowball1->Launch(Start, End);
                        
                        Snowball2->EnemyManager = this;
                        Snowball2->ProjectedDirection = EFaceDirection::Xminus;
                        Snowball2->Launch(End, Start);
                    }
                }
            }
        }
    }
}

void AGoEnemyManager::SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass, AGoTile* TileRef, EFaceDirection Direction)
{
    AGoPawnEnemy* NewEnemy = GetWorld()->SpawnActor<AGoPawnEnemy>(EnemyClass, Transform);
    NewEnemy->TileManager = TileManager;
    NewEnemy->CurrTile = TileRef;
    NewEnemy->Direction = Direction;
    NewEnemy->EnemyManager = this;
    NewEnemy->OnEnemyDeath.AddDynamic(this, &AGoEnemyManager::RemoveEnemyFromList);
    Enemies.Add(NewEnemy);
    
    OnEnemySpawned(NewEnemy);
}

void AGoEnemyManager::FinishEnemyTurn()
{
	CheckSnowmanAttacks();
    
	for (AGoPawnEnemy* Enemy : Enemies) 
	{
		Enemy->OnPostMove();
	}
    
	if (PlayerRef && PlayerRef->CurrentJakeTile)
	{
		for (AGoPawnEnemy* Enemy : Enemies)
		{
			if (Enemy->CurrTile == PlayerRef->CurrentJakeTile)
			{
				PlayerRef->RegisterEntityOnJakeTile(Enemy);
			}
			else
			{
				PlayerRef->UnregisterEntityOnJakeTile(Enemy);
			}
		}
	}
    
	TryEndTurn();
}

void AGoEnemyManager::RemoveEnemyFromList(AGoPawnEnemy* EnemyRef)
{
    if(!Enemies.Contains(EnemyRef)) return;
    Enemies.Remove(EnemyRef);
    if(EnemyRef->IsA(AGoPawnEnemyGunter::StaticClass())) 
    {
        GunterIsDead.Broadcast();
    }
}

void AGoEnemyManager::UpdateOccupancy()
{
    OccupiedTiles.Empty();
    
    if (PlayerRef && PlayerRef->CurrTile) {PlayerTileIndex = PlayerRef->CurrTile->Index;}
    else {PlayerTileIndex = -1;}
    
    for (AGoPawnEnemy* Enemy : Enemies)
    {
	    if (Enemy && Enemy->CurrTile) {OccupiedTiles.Add(Enemy->CurrTile->Index, Enemy);}
    	if (Enemy->IsA(AGoPawnEnemyGunter::StaticClass()) && Enemy->CurrTile->TileType == ETileType::End) {GunterEnd.Broadcast();}
    }
}

bool AGoEnemyManager::IsTileOccupied(int TileIndex, const AGoPawnEnemy* ExcludeEnemy) const
{
    if (TileIndex == PlayerTileIndex) return true;
    
    if (const AGoPawnEnemy* const* Occupant = OccupiedTiles.Find(TileIndex)) {return (*Occupant) != ExcludeEnemy;}

	return false;
}

void AGoEnemyManager::RegisterSnowball(AGoSnowball* Snowball)
{
    if (Snowball) {ActiveSnowballs.Add(Snowball);}
}

void AGoEnemyManager::UnregisterSnowball(AGoSnowball* Snowball)
{
    if (Snowball) {ActiveSnowballs.Remove(Snowball);}
    
    if (bWaitingToEndTurn && ActiveSnowballs.Num() == 0) {TryEndTurn();}
}

void AGoEnemyManager::TryEndTurn()
{
    if (ActiveSnowballs.Num() == 0)
    {
    	for (AGoPawnEnemy* Enemy : Enemies)
    	{
    		if (AGoPawnEnemyGunter* Gunter = Cast<AGoPawnEnemyGunter>(Enemy))
    		{
    			Gunter->ExecuteQueuedMove();
    		}
    	}
    	EndTurn();
    }
    else {bWaitingToEndTurn = true;}
}

void AGoEnemyManager::EndTurn()
{
	if(bIsEndingTurn){return;}
	bIsEndingTurn = true;
	bWaitingToEndTurn = false;
	bIsProcessingTurn = false;
	
    UpdateOccupancy();
	
	NoRemainingEnemyTurns.Broadcast();
	UE_LOG(LogTemp, Display, TEXT("Enemy turn ended"));
}