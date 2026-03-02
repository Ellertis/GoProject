// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoEnemyManager.h"

#include "Gameplay/Enemies/GoPawnEnemyGunter.h"
#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "Player/GoPawnPlayer.h"

// Sets default values
AGoEnemyManager::AGoEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGoEnemyManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGoEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoEnemyManager::CheckSnowmanAttacks()
{
    TArray<AGoPawnEnemySnowmen*> Snowmen;
    for (AGoPawnEnemy* Enemy : Enemies)
    {
        if (AGoPawnEnemySnowmen* Snowman = Cast<AGoPawnEnemySnowmen>(Enemy))
            Snowmen.Add(Snowman);
    }
    if (Snowmen.Num() < 2) return;
	
    // Group by X (vertical lines) and Y (horizontal lines)
    TMap<int, TArray<AGoPawnEnemySnowmen*>> ByX;
    TMap<int, TArray<AGoPawnEnemySnowmen*>> ByY;
    for (AGoPawnEnemySnowmen* S : Snowmen)
    {
        if (!S->CurrTile) continue;
        FIntPoint Pos = TileManager->Get2DIndex(S->CurrTile->Index);
        ByX.FindOrAdd(Pos.X).Add(S);
        ByY.FindOrAdd(Pos.Y).Add(S);
    }
	
	// Vertical alignments (same X)
	for (const TTuple<int, TArray<AGoPawnEnemySnowmen*>>& Pair : ByX)
	{
		const TArray<AGoPawnEnemySnowmen*>& Group = Pair.Value;
		if (Group.Num() < 2) continue;
		
		for (int i = 0; i < Group.Num(); i++)
		{
			for (int j = i + 1; j < Group.Num(); j++)
			{
				AGoPawnEnemySnowmen* S1 = Group[i];
				AGoPawnEnemySnowmen* S2 = Group[j];
				
				FVector Start = S1->GetActorLocation();
				FVector End = S2->GetActorLocation();
				
				AGoSnowball* Snowball = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, Start, FRotator::ZeroRotator);
				Snowball->EnemyManager = this;
				Snowball->ProjectedDirection = EFaceDirection::Xplus; //Can be Xminus but should be irrelevant
				AGoSnowball* Snowball2 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, End, FRotator::ZeroRotator);
				Snowball2->EnemyManager = this;
				Snowball->ProjectedDirection = EFaceDirection::Xplus;
				if (Snowball && Snowball2)
				{
					Snowball->Launch(Start, End);
					Snowball2->Launch(End, Start);
				}
			}
		}
	}

	// Horizontal alignments (same Y)
	for (const TTuple<int, TArray<AGoPawnEnemySnowmen*>>& Pair : ByY)
	{
		const TArray<AGoPawnEnemySnowmen*>& Group = Pair.Value;
		if (Group.Num() < 2) continue;

		for (int i = 0; i < Group.Num(); i++)
		{
			for (int j = i + 1; j < Group.Num(); j++)
			{
				AGoPawnEnemySnowmen* S1 = Group[i];
				AGoPawnEnemySnowmen* S2 = Group[j];

				FVector Start = S1->GetActorLocation();
				FVector End = S2->GetActorLocation();
				
				AGoSnowball* Snowball = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, Start, FRotator::ZeroRotator);
				Snowball->EnemyManager = this;
				Snowball->ProjectedDirection = EFaceDirection::Yplus; //Can be Xminus but should be irrelevant
				AGoSnowball* Snowball2 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, End, FRotator::ZeroRotator);
				Snowball2->EnemyManager = this;
				Snowball->ProjectedDirection = EFaceDirection::Yplus;
				if (Snowball && Snowball2)
				{
					Snowball->Launch(Start, End);
					Snowball2->Launch(End, Start);
				}
			}
		}
	}
}

void AGoEnemyManager::SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass,AGoTile* TileRef,EFaceDirection Direction)
{
	AGoPawnEnemy* NewEnemy = GetWorld()->SpawnActor<AGoPawnEnemy>(EnemyClass, Transform);
	NewEnemy->TileManager = TileManager;
	NewEnemy->CurrTile = TileRef;
	NewEnemy->Direction = Direction;
	NewEnemy->EnemyManager = this;
	NewEnemy->OnEnemyDeath.AddDynamic(this,&AGoEnemyManager::RemoveEnemyFromList);
	Enemies.Add(NewEnemy);
}

void AGoEnemyManager::RemoveEnemyFromList(AGoPawnEnemy* EnemyRef)
{
	if(!Enemies.Contains(EnemyRef)) return;
	Enemies.Remove(EnemyRef);
	if(EnemyRef->IsA(AGoPawnEnemyGunter::StaticClass())) GunterIsDead.Broadcast(); // If Gunter dies, game over
}

void AGoEnemyManager::OnNewEnemyTurn(const ETurnPhase NewTurnPhase)
{
	if (NewTurnPhase != ETurnPhase::EnemyTurn) return;

	bGunterNeedsMove = false;
	bWaitingToEndTurn = false;
	
	TArray<AGoPawnEnemySnowmen*> Snowmen;
	AGoPawnEnemyGunter* Gunter = nullptr;

	for (AGoPawnEnemy* Enemy : Enemies)
	{
		if (AGoPawnEnemySnowmen* Snowman = Cast<AGoPawnEnemySnowmen>(Enemy))
		{
			Snowmen.Add(Snowman);
		}
		else if (AGoPawnEnemyGunter* G = Cast<AGoPawnEnemyGunter>(Enemy))
		{
			Gunter = G;
		}
	}
	
	//Pre-turn updates for all enemies (player adjacency check for Gunter)
	for (AGoPawnEnemy* Enemy : Enemies) {Enemy->PreTurnUpdate();}

	//Set Player and Enemies occupied tiles
	UpdateOccupancy();
	
	//Gunter ComputeMove | ApplyMoveIntent
	if (Gunter)
	{
		FEnemyMoveIntent GunterIntent = Gunter->ComputeMoveIntent();
		Gunter->ApplyMoveIntent(GunterIntent);
	}
	
	UpdateOccupancy();
	
	//Snowmen ComputeMove | ApplyMoveIntent
	for (AGoPawnEnemySnowmen* Snowman : Snowmen)
	{
		FEnemyMoveIntent SnowmanIntent = Snowman->ComputeMoveIntent();
		Snowman->ApplyMoveIntent(SnowmanIntent);
	}

	//Check snowman attacks
	CheckSnowmanAttacks();

	//Check if Gunter was hit with snowballs
	if (Gunter && Gunter->WasHitThisTurn())
	{
		Gunter->ClearHitFlag();
		FEnemyMoveIntent GunterIntent = Gunter->ComputeMoveIntent();
		Gunter->ApplyMoveIntent(GunterIntent);
		UpdateOccupancy();
	}
	
	for (AGoPawnEnemy* Enemy : Enemies) 
	{
		Enemy->OnPostMove();
	}
	
	TryEndTurn();
}

void AGoEnemyManager::UpdateOccupancy()
{
	OccupiedTiles.Empty();
	
	if (PlayerRef && PlayerRef->CurrTile) {PlayerTileIndex = PlayerRef->CurrTile->Index;}
	else{PlayerTileIndex = -1;}
    
	// Add all enemies
	for (AGoPawnEnemy* Enemy : Enemies)
	{
		if (Enemy && Enemy->CurrTile)
		{
			OccupiedTiles.Add(Enemy->CurrTile->Index, Enemy);
		}
	}
}

bool AGoEnemyManager::IsTileOccupied(int TileIndex, const AGoPawnEnemy* ExcludeEnemy) const
{
	if (TileIndex == PlayerTileIndex) return true;
	
	if (const AGoPawnEnemy* const* Occupant = OccupiedTiles.Find(TileIndex))
	{
		return (*Occupant) != ExcludeEnemy;
	}
	return false;
}

void AGoEnemyManager::RegisterSnowball(AGoSnowball* Snowball)
{
	if (Snowball){ActiveSnowballs.Add(Snowball);}
}

void AGoEnemyManager::UnregisterSnowball(AGoSnowball* Snowball)
{
	if (Snowball){ActiveSnowballs.Remove(Snowball);}
	if (bGunterNeedsMove){ProcessGunterReaction();}
	if (bWaitingToEndTurn && ActiveSnowballs.Num() == 0)
	{
		EndTurn();
	}
}

void AGoEnemyManager::EndTurn()
{
	bWaitingToEndTurn = false;
	NoRemainingEnemyTurns.Broadcast();
}

void AGoEnemyManager::TryEndTurn()
{
	if (ActiveSnowballs.Num() == 0)
	{
		EndTurn();
	}
	else
	{
		bWaitingToEndTurn = true;
	}
}

void AGoEnemyManager::ProcessGunterReaction()
{
	if (!bGunterNeedsMove) return;
	
	AGoPawnEnemyGunter* Gunter = nullptr;
	for (AGoPawnEnemy* Enemy : Enemies)
	{
		if (AGoPawnEnemyGunter* G = Cast<AGoPawnEnemyGunter>(Enemy))
		{
			Gunter = G;
			break;
		}
	}
    
	if (Gunter && Gunter->IsFleeing())
	{
		UE_LOG(LogTemp, Warning, TEXT("Processing Gunter reaction move"));
		FEnemyMoveIntent GunterIntent = Gunter->ComputeMoveIntent();
		Gunter->ApplyMoveIntent(GunterIntent);
		UpdateOccupancy();
		bGunterNeedsMove = false;
	}
}
