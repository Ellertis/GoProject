// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoPawnEnemy.h"
#include "GameFramework/Actor.h"
#include "Gameplay/GoTurnManager.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "GoEnemyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRemainingEnemyTurns);

UCLASS()
class GOPROJECT_API AGoEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoEnemyManager();

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<AGoPawnEnemy*> Enemies;

	UPROPERTY()
	int EnemyTurnsRemaining = 0;

	UPROPERTY()
	AGoTurnManager* TurnManager;

	UPROPERTY()
	AGoTileManager* TileManager;

	FNoRemainingEnemyTurns NoRemainingEnemyTurns;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass,AGoTile* TileRef,EFaceDirection Direction);

	UFUNCTION()
	void RemoveEnemyFromList(AGoPawnEnemy* EnemyRef);

	UFUNCTION()
	void OnEnemyMoved();

	UFUNCTION()
	void OnNewEnemyTurn(const ETurnPhase NewTurnPhase);
};
