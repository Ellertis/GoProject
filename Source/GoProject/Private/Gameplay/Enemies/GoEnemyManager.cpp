// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoEnemyManager.h"

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

void AGoEnemyManager::SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass)
{
	AGoPawnEnemy* NewEnemy = GetWorld()->SpawnActor<AGoPawnEnemy>(EnemyClass, Transform);
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
	//Keep track of the enemies that moved // finished their turn.
}

