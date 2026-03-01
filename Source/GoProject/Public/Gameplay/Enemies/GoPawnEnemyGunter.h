// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "Player/GoPawnPlayer.h"
#include "GoPawnEnemyGunter.generated.h"

UCLASS()
class GOPROJECT_API AGoPawnEnemyGunter : public AGoPawnEnemy
{
	GENERATED_BODY()

public:
	AGoPawnEnemyGunter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Pawn|Health")
	int StartHealth = 3;

	virtual void PreTurnUpdate_Implementation() override;
	
	virtual FEnemyMoveIntent ComputeMoveIntent_Implementation() const override;
	
	virtual void ApplyMoveIntent_Implementation(const FEnemyMoveIntent& Intent) override;
	
	virtual void OnPostMove_Implementation() override;

	virtual void ApplyDamage_Implementation(int Amount) override;

	void StartFleeing(bool bSetDirection, EFaceDirection AwayDir);

	int CountWalkableTilesInDirection(const FIntPoint& StartCoord, EFaceDirection Dir, int IgnoreTileIndex) const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Pawn|Movement")
	bool bIsFleeing = false;

	UPROPERTY()
	AGoPawnPlayer* PlayerRef = nullptr;

	UPROPERTY()
	AGoTile* PrevTile = nullptr;
};
