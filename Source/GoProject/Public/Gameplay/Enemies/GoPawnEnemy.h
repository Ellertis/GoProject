// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GoPawn.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "GoPawnEnemy.generated.h"

class AGoTileManager;
class AGoEnemyManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AGoPawnEnemy*,EnemyRef);

USTRUCT(BlueprintType)
struct FEnemyMoveIntent
{
	GENERATED_BODY()
	
	UPROPERTY()
	AGoTile* TargetTile = nullptr;
	UPROPERTY()
	EFaceDirection NewDirection = EFaceDirection::Xplus;
};

UCLASS(Abstract)
class GOPROJECT_API AGoPawnEnemy : public AGoPawn
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Enemy|Movement")
	void PreTurnUpdate();
	virtual void PreTurnUpdate_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, Category="Enemy|Movement")
	FEnemyMoveIntent ComputeMoveIntent() const;
	virtual FEnemyMoveIntent ComputeMoveIntent_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Enemy|Movement")
	void ApplyMoveIntent(const FEnemyMoveIntent& Intent);
	virtual void ApplyMoveIntent_Implementation(const FEnemyMoveIntent& Intent);

	UFUNCTION(BlueprintNativeEvent, Category = "Enemy|Movement")
	void OnPostMove();
	virtual void OnPostMove_Implementation();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy|Movement")
	EFaceDirection Direction;
	
	UFUNCTION(BlueprintNativeEvent, Category="Enemy|Health")
	void ApplyDamage(int Amount, EFaceDirection HitDirection);
	virtual void ApplyDamage_Implementation(int Amount, EFaceDirection HitDirection);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Enemy|Health")
	int Health;

	//HELPERS
	FIntPoint GetDirectionDelta(const EFaceDirection DirectionValue) const;

	EFaceDirection GetDirectionFromDelta(FIntPoint& Delta) const;

	EFaceDirection GetOppositeDirection() const;
	
	UPROPERTY()
	AGoTileManager* TileManager;

	UPROPERTY()
	AGoEnemyManager* EnemyManager;
	
	FOnEnemyDeath OnEnemyDeath;
};
