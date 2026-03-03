// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/GoTurnManager.h"

AGoTurnManager::AGoTurnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGoTurnManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGoTurnManager::SetTurnPhase(ETurnPhase NewTurnPhase)
{
	UE_LOG(LogTemp, Display, TEXT("TurnManager: Setting phase from %d to %d"), (int)CurrentTurnPhase, (int)NewTurnPhase);
    
	CurrentTurnPhase = NewTurnPhase;
	OnTurnPhaseChanged.Broadcast(CurrentTurnPhase);
    
	// Call Blueprint events
	switch (CurrentTurnPhase)
	{
		case ETurnPhase::PlayerTurn: OnPlayerTurnStarted();break;
		case ETurnPhase::EnemyTurn: OnEnemyTurnStarted();break;
		case ETurnPhase::Resolve: OnTurnEnded();break;
		default: break;
	}
}

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