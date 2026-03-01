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
    // Gather all snowmen
    TArray<AGoPawnEnemySnowmen*> Snowmen;
    for (AGoPawnEnemy* Enemy : Enemies)
    {
        if (AGoPawnEnemySnowmen* Snowman = Cast<AGoPawnEnemySnowmen>(Enemy))
            Snowmen.Add(Snowman);
    }
    if (Snowmen.Num() < 2) return;
	UE_LOG(LogTemp, Warning, TEXT("Total snowmen: %d"), Snowmen.Num());

    // Group by X (vertical lines) and Y (horizontal lines)
    TMap<int, TArray<AGoPawnEnemySnowmen*>> ByX;
    TMap<int, TArray<AGoPawnEnemySnowmen*>> ByY;
    for (AGoPawnEnemySnowmen* S : Snowmen)
    {
        if (!S->CurrTile) continue;
        FIntPoint Pos = TileManager->Get2DIndex(S->CurrTile->Index);
    	UE_LOG(LogTemp, Warning, TEXT("Snowman at tile %d -> X=%d, Y=%d"), S->CurrTile->Index, Pos.X, Pos.Y);
        ByX.FindOrAdd(Pos.X).Add(S);
        ByY.FindOrAdd(Pos.Y).Add(S);
    }
	UE_LOG(LogTemp, Warning, TEXT("ByX groups: %d"), ByX.Num());
	UE_LOG(LogTemp, Warning, TEXT("ByY groups: %d"), ByY.Num());
	// Vertical alignments (same X)
	for (const TTuple<int, TArray<AGoPawnEnemySnowmen*>>& Pair : ByX)
	{
		const TArray<AGoPawnEnemySnowmen*>& Group = Pair.Value;
		UE_LOG(LogTemp, Warning, TEXT("Vertical group X=%d has %d snowmen"), Pair.Key, Group.Num());
		if (Group.Num() < 2) continue;
		
		for (int i = 0; i < Group.Num(); i++)
		{
			for (int j = i + 1; j < Group.Num(); j++)
			{
				AGoPawnEnemySnowmen* S1 = Group[i];
				AGoPawnEnemySnowmen* S2 = Group[j];
				UE_LOG(LogTemp, Warning, TEXT("  -> Spawning snowball between snowmen %d and %d"), i, j);
				FVector Start = S1->GetActorLocation();
				FVector End = S2->GetActorLocation();
				
				
				AGoSnowball* Snowball = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, Start, FRotator::ZeroRotator);
				AGoSnowball* Snowball2 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, End, FRotator::ZeroRotator);
				if (Snowball && Snowball2)
				{
					UE_LOG(LogTemp, Warning, TEXT("Snowball"));
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
				AGoSnowball* Snowball2 = GetWorld()->SpawnActor<AGoSnowball>(SnowballClass, End, FRotator::ZeroRotator);
				if (Snowball && Snowball2)
				{
					UE_LOG(LogTemp, Warning, TEXT("Snowball"));
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
	NewEnemy->OnEnemyDeath.AddDynamic(this,&AGoEnemyManager::RemoveEnemyFromList);
	Enemies.Add(NewEnemy);
	
}

void AGoEnemyManager::RemoveEnemyFromList(AGoPawnEnemy* EnemyRef)
{
	if(!Enemies.Contains(EnemyRef)) return;
	Enemies.Remove(EnemyRef);
	if(EnemyRef->StaticClass() == AGoPawnEnemyGunter::StaticClass()) GunterIsDead.Broadcast(); // If Gunter dies, game over
}

void AGoEnemyManager::OnNewEnemyTurn(const ETurnPhase NewTurnPhase)
{
	if (NewTurnPhase != ETurnPhase::EnemyTurn) return;

	//Pre turn updates
	for(AGoPawnEnemy* Enemy : Enemies) {Enemy->PreTurnUpdate();}
	
	//Compute enemies intents
	TArray<FEnemyMoveIntent> Intents;
	Intents.Reserve(Enemies.Num());
	for(const AGoPawnEnemy* Enemy : Enemies) {Intents.Add(Enemy->ComputeMoveIntent());}

	//Apply enemies intents
	for(int i = 0; i < Enemies.Num(); i++) {Enemies[i]->ApplyMoveIntent(Intents[i]);}

	//Post Move updates
	for(AGoPawnEnemy* Enemy : Enemies) {Enemy->OnPostMove();}

	//Check snowmen alignments
	CheckSnowmanAttacks();

	NoRemainingEnemyTurns.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("BROADCASTED NO ENEMY TURNS LEFT"));
}
