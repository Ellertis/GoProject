// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "GoPawnEnemySnowmen.generated.h"

UCLASS(Blueprintable)
class GOPROJECT_API AGoPawnEnemySnowmen : public AGoPawnEnemy
{
	GENERATED_BODY()

public:
	virtual FMoveIntent ComputeMoveIntent_Implementation() const override;
	
	virtual void ApplyMoveIntent_Implementation(const FMoveIntent& Intent) override;
	
	virtual void OnPostMove_Implementation() override;
};