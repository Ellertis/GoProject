// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Enemies/GoPawnEnemy.h"
#include "GoPawnEnemyGunter.generated.h"

class AGoPawnPlayer;

UCLASS(Blueprintable)
class GOPROJECT_API AGoPawnEnemyGunter : public AGoPawnEnemy
{
    GENERATED_BODY()

public:
    AGoPawnEnemyGunter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gunter")
    int Health;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gunter")
    int StartHealth = 3;

    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoPawnPlayer* PlayerRef;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    AGoTile* PrevTile;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bIsFleeing;
    
    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bWasHitThisTurn;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    EFaceDirection PostHitDirection;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    TArray<EFaceDirection> PreferredFleeOrder;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    EFaceDirection FleeDirection;

    virtual void PreTurnUpdate_Implementation() override;
    virtual FMoveIntent ComputeMoveIntent_Implementation() const override;
    virtual void ApplyMoveIntent_Implementation(const FMoveIntent& Intent) override;
    virtual void OnPostMove_Implementation() override;
    virtual void ApplyDamage_Implementation(int Amount, EFaceDirection HitDirection) override;

    UFUNCTION(BlueprintCallable)
    void StartFleeing(bool bSetDirection = true, EFaceDirection AwayDir = EFaceDirection::Xplus);
    
    UFUNCTION(BlueprintCallable)
    void ClearHitFlag() { bWasHitThisTurn = false; }

	UFUNCTION(BlueprintNativeEvent)
	void PlayFearAnimation();

	UFUNCTION(BlueprintCallable)
	void OnFearAnimationComplete();

    UPROPERTY(BlueprintReadWrite, Category = "State")
    bool bHasPendingFlee = false;

    UPROPERTY(BlueprintReadWrite, Category = "State")
    EFaceDirection PendingFleeDirection;
	
	bool CanMoveToTileIndex(int TileIndex) const;

	EFaceDirection GetBestFleeDirection() const;

	int CountWalkableTilesInDirection(const FIntPoint& StartCoord, EFaceDirection Dir, int IgnoreTileIndex) const;

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY()
    class UGoPathfindingSubsystem* PathfindingSubsystem;
};