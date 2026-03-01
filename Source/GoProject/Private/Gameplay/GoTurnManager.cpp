// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GoTurnManager.h"

// Sets default values
AGoTurnManager::AGoTurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGoTurnManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGoTurnManager::SetTurnPhase(ETurnPhase NewTurnPhase)
{
	CurrentTurnPhase = NewTurnPhase;
	OnTurnPhaseChanged.Broadcast(CurrentTurnPhase);
	UE_LOG(LogTemp, Display, TEXT("Turn Phase Changed"));
}

// Called every frame
void AGoTurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoTurnManager::StartGame()
{
	SetTurnPhase(ETurnPhase::PlayerTurn);
}

void AGoTurnManager::PlayerMoved()
{
	SetTurnPhase(ETurnPhase::EnemyTurn);
}

void AGoTurnManager::OnNoEnemyTurnsLeft()
{
	SetTurnPhase(ETurnPhase::PlayerTurn);
}
