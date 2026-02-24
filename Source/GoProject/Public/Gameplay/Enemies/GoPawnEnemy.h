// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GoPawn.h"
#include "GoPawnEnemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyMovement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AGoPawnEnemy*,EnemyRef);

UCLASS()
class GOPROJECT_API AGoPawnEnemy : public AGoPawn
{
	GENERATED_BODY()

public:

	FOnEnemyMovement OnEnemyMovement;
	
	FOnEnemyDeath OnEnemyDeath;

	
};
