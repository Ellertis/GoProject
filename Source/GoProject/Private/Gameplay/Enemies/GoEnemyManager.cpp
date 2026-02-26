// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoEnemyManager.h"

#include "Gameplay/Tile/BoardDataAsset.h"

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

void AGoEnemyManager::SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass,AGoTile* TileRef,EFaceDirection Direction)
{
	AGoPawnEnemy* NewEnemy = GetWorld()->SpawnActor<AGoPawnEnemy>(EnemyClass, Transform);
	NewEnemy->TileManager = TileManager;
	NewEnemy->CurrTile = TileRef;
	NewEnemy->Direction = Direction;
	NewEnemy->OnEnemyDeath.AddDynamic(this,&AGoEnemyManager::RemoveEnemyFromList);
	NewEnemy->OnEnemyMovement.AddDynamic(this,&AGoEnemyManager::OnEnemyMoved);
	Enemies.Add(NewEnemy);
	
}

void AGoEnemyManager::RemoveEnemyFromList(AGoPawnEnemy* EnemyRef)
{
	if(!Enemies.Contains(EnemyRef)) return;
	Enemies.Remove(EnemyRef);
}

void AGoEnemyManager::OnEnemyMoved()
{
	if(EnemyTurnsRemaining<0){UE_LOG(LogTemp,Error,TEXT("EnemyManager : EnemyTurnsRemaining is less than actual Enemies, but enemy still tried to move. Tracking issue")) return;};
	EnemyTurnsRemaining--;
	if(EnemyTurnsRemaining==0)
	{
		NoRemainingEnemyTurns.Broadcast();
		UE_LOG(LogTemp,Warning,TEXT("BROADCASTED NO ENEMY TURNS LEFT"));
		return;
		//Keep track of the enemies that moved || finished their turn.
	}
	UE_LOG(LogTemp,Warning,TEXT("DID NOT BROADCASTED/ NO ENEMY TURNS LEFT"));
}

void AGoEnemyManager::OnNewEnemyTurn(const ETurnPhase NewTurnPhase)
{
	if(NewTurnPhase != ETurnPhase::EnemyTurn) return;
	EnemyTurnsRemaining = Enemies.Num();
	for(AGoPawnEnemy* Enemy : Enemies)
	{
		Enemy->StartTurn();
	}
}
