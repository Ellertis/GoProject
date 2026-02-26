// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GoPawn.h"
#include "GoPawnEnemy.generated.h"

class AGoTileManager;
class UBoardDataAsset;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyMovement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AGoPawnEnemy*,EnemyRef);

UENUM(BlueprintType)
enum class EFaceDirection : uint8
{
	Xplus,
	Xminus,
	Yplus,
	Yminus
};

UCLASS()
class GOPROJECT_API AGoPawnEnemy : public AGoPawn
{
	GENERATED_BODY()

public:
	void StartTurn();
	void FinishTurn();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Movement")
	EFaceDirection Direction;

	UPROPERTY()
	AGoTileManager* TileManager;
	
	FOnEnemyMovement OnEnemyMovement;
	
	FOnEnemyDeath OnEnemyDeath;
protected:
	FIntPoint GetDirectionDelta() const;

	EFaceDirection GetOppositeDirection() const;

	void EnemyMovement();
};
