// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoTurnManager.generated.h"

UENUM()
enum class ETurnPhase: uint8
{
	None,
	PlayerTurn,
	EnemyTurn,
	Resolve
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnPhaseChanged,ETurnPhase,NewTurnPhase);

UCLASS()
class GOPROJECT_API AGoTurnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoTurnManager();

	void StartGame();

	void PlayerMoved();

	UFUNCTION()
	void OnNoEnemyTurnsLeft();
	
	// Variables
	UPROPERTY()
	ETurnPhase CurrentTurnPhase = ETurnPhase::None;

	// Delegates
	UPROPERTY()
	FOnTurnPhaseChanged OnTurnPhaseChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetTurnPhase(ETurnPhase NewTurnPhase);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
