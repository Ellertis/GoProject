// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoPawnEnemy.h"
#include "GoSnowball.h"
#include "GameFramework/Actor.h"
#include "Gameplay/GoTurnManager.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "GoEnemyManager.generated.h"

class AGoPawnPlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRemainingEnemyTurns);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGunterIsDead);

UCLASS()
class GOPROJECT_API AGoEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoEnemyManager();

	UPROPERTY(EditAnywhere, Category="Classes")
	TSubclassOf<AGoSnowball> SnowballClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<AGoPawnEnemy*> Enemies;
	
	UPROPERTY()
	AGoTurnManager* TurnManager;

	UPROPERTY()
	AGoTileManager* TileManager;

	UPROPERTY()
	AGoPawnPlayer* PlayerRef = nullptr;

	FNoRemainingEnemyTurns NoRemainingEnemyTurns;
	
	FGunterIsDead GunterIsDead;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CheckSnowmanAttacks();
	
	UPROPERTY()
	TMap<int, AGoPawnEnemy*> OccupiedTiles;

	UPROPERTY()
	int PlayerTileIndex = -1;

	UPROPERTY()
	TArray<AGoSnowball*> ActiveSnowballs;

	bool bWaitingToEndTurn = false;

	bool bGunterNeedsMove = false;
	
	void TryEndTurn();

	void ProcessGunterReaction();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass,AGoTile* TileRef,EFaceDirection Direction);

	UFUNCTION()
	void RemoveEnemyFromList(AGoPawnEnemy* EnemyRef);

	UFUNCTION()
	void OnNewEnemyTurn(const ETurnPhase NewTurnPhase);

	UFUNCTION()
	void UpdateOccupancy();
	
	UFUNCTION()
	bool IsTileOccupied(int TileIndex, const AGoPawnEnemy* ExcludeEnemy = nullptr) const;

	UFUNCTION()
	void RegisterSnowball(AGoSnowball* Snowball);

	UFUNCTION()
	void UnregisterSnowball(AGoSnowball* Snowball);

	UFUNCTION()
	void EndTurn();
};
