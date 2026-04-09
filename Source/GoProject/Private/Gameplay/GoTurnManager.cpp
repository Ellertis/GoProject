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
	CurrentTurnPhase = NewTurnPhase;
	OnTurnPhaseChanged.Broadcast(CurrentTurnPhase);
	
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