// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GoPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Player/GoCameraActor.h"
#include "Player/GoPawnPlayer.h"
#include "GoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GOPROJECT_API AGoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AGoCameraActor> CameraActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<AGoPawnPlayer> PlayerPawnClass;

private:
	UPROPERTY()
	AGoCameraActor* CameraActor;

	UPROPERTY()
	AGoPawnPlayer* PlayerPawn;

	UPROPERTY()
	AGoPlayerController* PlayerController;

	void SpawnPlayer();

	void SpawnCamera();
	
};
