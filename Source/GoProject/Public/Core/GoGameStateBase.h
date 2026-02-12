// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GoGameStateBase.generated.h"

UENUM()
enum class ETurnPhase: uint8
{
	None,
	PlayerTurn,
	EnemyTurn,
	Resolve
};
UCLASS()
class GOPROJECT_API AGoGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ETurnPhase CurrentTurnPhase;
};
