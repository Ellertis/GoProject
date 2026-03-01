// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Player/GoCameraActor.h"
#include "Player/GoPawnPlayer.h"
#include "GoGameModeBase.generated.h"

UCLASS()
class GOPROJECT_API AGoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	void GameOver();

	UFUNCTION()
	AGoPawnPlayer* GetPlayer() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AGoCameraActor> CameraActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AGoPawnPlayer> PlayerPawnClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AGoEnemyManager> EnemyManagerClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AGoTurnManager> TurnManagerClass;
	
	UPROPERTY()
	AGoEnemyManager* EnemyManager;

	UPROPERTY()
	AGoTurnManager* TurnManager;

private:
	UPROPERTY()
	AGoCameraActor* CameraActor;

	UPROPERTY()
	AGoPawnPlayer* PlayerPawn;

	UPROPERTY()
	AGoPlayerController* PlayerController;

	UPROPERTY()
	AGoTileManager* TM;

	UFUNCTION()
	void OnGridGenerated();
	
	void SpawnPlayer();

	void SpawnCamera();
	
	void SpawnEnemyManager();
	
	void SpawnTurnManager();
};
