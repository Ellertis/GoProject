// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoEnemyManager.generated.h"

enum class ETurnPhase : uint8;
class AGoSnowball;
class AGoTileManager;
class AGoPawnPlayer;
class AGoPawnEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRemainingEnemyTurns);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGunterIsDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGunterEnd);

UCLASS(Blueprintable)
class GOPROJECT_API AGoEnemyManager : public AActor
{
    GENERATED_BODY()

public:
    AGoEnemyManager();

    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoTileManager* TileManager;
    
    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoPawnPlayer* PlayerRef;

    UPROPERTY(BlueprintReadOnly, Category = "Enemies")
    TArray<AGoPawnEnemy*> Enemies;

    UPROPERTY(BlueprintReadOnly, Category = "Enemies")
    TMap<int, AGoPawnEnemy*> OccupiedTiles;
    
    UPROPERTY(BlueprintReadOnly, Category = "Enemies")
    int PlayerTileIndex = -1;

    UPROPERTY(BlueprintReadOnly, Category = "Snowball")
    TArray<AGoSnowball*> ActiveSnowballs;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Snowball")
    TSubclassOf<AGoSnowball> SnowballClass;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FNoRemainingEnemyTurns NoRemainingEnemyTurns;
    
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FGunterIsDead GunterIsDead;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGunterEnd GunterEnd;
    
    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bWaitingToEndTurn;
    
    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsProcessingTurn;

	UPROPERTY()
	int PendingGunterMoves = 0;

	UPROPERTY()
	int PendingSnowmanMoves = 0;

	UFUNCTION()
	void OnEnemyMoveCompleted(AGoPawnEnemy* Enemy);
	
    UFUNCTION()
    void OnNewEnemyTurn(const ETurnPhase NewTurnPhase);

	UFUNCTION()
	void SnowmanPhase(TArray<AGoPawnEnemySnowmen*>& Snowmen);

	UFUNCTION()
	void FinishEnemyTurn();
    
    UFUNCTION()
    void RemoveEnemyFromList(AGoPawnEnemy* EnemyRef);

    UFUNCTION(BlueprintCallable)
    void SpawnEnemy(FTransform Transform, TSubclassOf<AGoPawnEnemy> EnemyClass, AGoTile* TileRef, EFaceDirection Direction);
    
    UFUNCTION(BlueprintCallable)
    void UpdateOccupancy();
    
    UFUNCTION(BlueprintCallable)
    bool IsTileOccupied(int TileIndex, const AGoPawnEnemy* ExcludeEnemy = nullptr) const;

    UFUNCTION(BlueprintCallable)
    void RegisterSnowball(AGoSnowball* Snowball);
    
    UFUNCTION(BlueprintCallable)
    void UnregisterSnowball(AGoSnowball* Snowball);
    
    UFUNCTION(BlueprintCallable)
    void TryEndTurn();
    
    UFUNCTION(BlueprintCallable)
    void EndTurn();
	
    UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
    void OnSnowmanAttack(const FVector& Start, const FVector& End, EFaceDirection Direction);
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
    void OnEnemySpawned(AGoPawnEnemy* Enemy);
    
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void CheckSnowmanAttacks();
    
    UPROPERTY()
    class UGoPathfindingSubsystem* PathfindingSubsystem;
};