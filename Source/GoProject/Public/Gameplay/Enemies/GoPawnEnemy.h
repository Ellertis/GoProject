// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GoPawn.h"
#include "GoPawnEnemy.generated.h"

class AGoEnemyManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AGoPawnEnemy*, EnemyRef);

UCLASS(Blueprintable)
class GOPROJECT_API AGoPawnEnemy : public AGoPawn
{
    GENERATED_BODY()

public:
    AGoPawnEnemy();

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnEnemyDeath OnEnemyDeath;
	
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    void PreTurnUpdate();
    virtual void PreTurnUpdate_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FMoveIntent ComputeMoveIntent() const;
    virtual FMoveIntent ComputeMoveIntent_Implementation() const;

    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    void ApplyMoveIntent(const FMoveIntent& Intent);
    virtual void ApplyMoveIntent_Implementation(const FMoveIntent& Intent);

    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    void OnPostMove();
    virtual void OnPostMove_Implementation();
	
    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    void ApplyDamage(int Amount, EFaceDirection HitDirection);
    virtual void ApplyDamage_Implementation(int Amount, EFaceDirection HitDirection);
	
    UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
    void OnDamageTaken(int Amount, EFaceDirection HitDirection);
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
    void OnDeath();

    virtual bool CanMoveToTile(AGoTile* Tile) const override;
	
    bool CanMoveToTileIndex(int TileIndex) const;
};