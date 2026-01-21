// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/GoCameraPawn.h"
#include "Player/PlayerActor.h"
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
	TSubclassOf<AGoCameraPawn> CameraPawnClass;

	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TSubclassOf<APlayerActor> PlayerActorClass;

private:
	UPROPERTY()
	AGoCameraPawn* CameraPawn;

	UPROPERTY()
	APlayerActor* PlayerActor;

	void SpawnPlayer();

	void SpawnCameraPawn();
};
