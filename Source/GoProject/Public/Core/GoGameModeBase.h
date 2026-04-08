// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Gameplay/GoTurnManager.h"
#include "Core/GoPlayerController.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "Player/GoPawnPlayer.h"
#include "Player/GoCameraActor.h"
#include "GoGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameWin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverFinn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverGunter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSandwichUpdate, int, Count);

UCLASS()
class GOPROJECT_API AGoGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void GameOver();

    UFUNCTION(BlueprintCallable)
    void GameWin();

    UFUNCTION(BlueprintCallable)
    void GameOverGunter();

    UFUNCTION(BlueprintCallable)
    void GameOverFinn();

    UFUNCTION(BlueprintCallable)
    void GameStart();

    UFUNCTION(BlueprintCallable)
    AGoPawnPlayer* GetPlayer() const { return PlayerPawn; }
    
    UFUNCTION(BlueprintCallable)
    void RestartLevel();
	
    UFUNCTION(BlueprintCallable)
    int GetSandwichCount() const { return SandwichCount; }
    
    UFUNCTION(BlueprintCallable)
    void AddSandwich(int Amount) { SandwichCount += Amount; OnSandwichUpdate.Broadcast(SandwichCount); }
    
    UFUNCTION(BlueprintCallable)
    bool UseSandwich() { if (SandwichCount > 0) { SandwichCount--; OnSandwichUpdate.Broadcast(SandwichCount); return true; } return false; }
	
    UPROPERTY(BlueprintAssignable)
    FOnGameWin OnGameWin;
    
    UPROPERTY(BlueprintAssignable)
    FOnGameOver OnGameOver;

    UPROPERTY(BlueprintAssignable)
    FOnGameStart OnGameStart;

    UPROPERTY(BlueprintAssignable)
    FOnSandwichUpdate OnSandwichUpdate;

    UPROPERTY(BlueprintAssignable)
    FOnGameOverFinn OnGameOverFinn;

    UPROPERTY(BlueprintAssignable)
    FOnGameOverGunter OnGameOverGunter;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Managers")
    TSubclassOf<AGoEnemyManager> EnemyManagerClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Managers")
    TSubclassOf<AGoTurnManager> TurnManagerClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
    TSubclassOf<AGoPawnPlayer> PlayerPawnClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    TSubclassOf<AGoCameraActor> CameraActorClass;

    UPROPERTY()
    AGoTileManager* TM;
    
    UPROPERTY()
    AGoEnemyManager* EnemyManager;
    
    UPROPERTY()
    AGoTurnManager* TurnManager;
    
    UPROPERTY()
    AGoPawnPlayer* PlayerPawn;
    
    UPROPERTY()
    AGoPlayerController* PlayerController;
    
    UPROPERTY()
    AGoCameraActor* CameraActor;

    int SandwichCount = 0;

    UFUNCTION()
    void OnGridGenerated();
    
    void SpawnEnemyManager();
    void SpawnTurnManager();
    void SpawnPlayer();
    void SpawnCamera();
};