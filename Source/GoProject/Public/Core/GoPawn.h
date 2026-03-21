// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "GoPawn.generated.h"

class AGoEnemyManager;
class AGoTurnManager;

USTRUCT(BlueprintType)
struct FMoveIntent
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    AGoTile* TargetTile = nullptr;

    UPROPERTY(BlueprintReadWrite)
    EFaceDirection NewDirection = EFaceDirection::Xplus;
};

UENUM(BlueprintType)
enum EMovementStyle
{
	Jump UMETA(DisplayName = "Jump"),
	Slide UMETA(DisplayName = "Slide")
};

UCLASS(Blueprintable)
class GOPROJECT_API AGoPawn : public APawn
{
    GENERATED_BODY()

public:
    AGoPawn();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoTileManager* TileManager;
    
    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoEnemyManager* EnemyManager;
    
    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoTurnManager* TurnManager;
    
    UPROPERTY(BlueprintReadWrite, Category = "State")
    AGoTile* CurrTile;

	UPROPERTY(BlueprintReadWrite, Category = "State")
	EFaceDirection Direction;
	
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float MoveDuration = 0.5f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	AGoTile* TargetTile;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector StartLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector TargetLocation;
	
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float JumpHeight = 100;
	
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float SpawnHeight = 100;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	TEnumAsByte<EMovementStyle> MovementStyle = Jump;
	
	UPROPERTY()
	FTimerHandle MoveTimerHandle;
	
	UPROPERTY()
	float MoveStartTime = 0.0f;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartMoveToTile(AGoTile* Tile, float Duration);
	
	UFUNCTION()
	void UpdateMove();
	
	UFUNCTION()
	void FinishMove();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	float GetJumpHeight(float Alpha);
	virtual float GetJumpHeight_Implementation(float Alpha);
	
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual bool CanMoveToTile(AGoTile* Tile) const;
    
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual TArray<AGoTile*> GetValidMoveTiles() const;
    
    UFUNCTION(BlueprintNativeEvent, Category = "Movement")
    void OnMoveStart();
    virtual void OnMoveStart_Implementation();
    
    UFUNCTION(BlueprintNativeEvent, Category = "Movement")
    void OnMoveEnd();
    virtual void OnMoveEnd_Implementation();
    
    UFUNCTION(BlueprintNativeEvent, Category = "Movement")
    void OnMoveToTile(AGoTile* Tile);
    virtual void OnMoveToTile_Implementation(AGoTile* Tile);
	
    UFUNCTION(BlueprintCallable, Category = "Direction")
    FIntPoint GetDirectionDelta(EFaceDirection DirectionValue) const;

    UFUNCTION(BlueprintCallable, Category = "Direction")
    EFaceDirection GetDirectionFromDelta(const FIntPoint& Delta) const;

    UFUNCTION(BlueprintCallable, Category = "Direction")
    EFaceDirection GetOppositeDirection(EFaceDirection Dir) const;
	
    UFUNCTION(BlueprintCallable, Category = "Position")
    FVector GetTilePosition(AGoTile* Tile) const;
	
    UFUNCTION(BlueprintCallable, Category = "Tile")
    AGoTile* GetTileFromIndex(int TileIndex) const;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float HeightOffset = 100.0f;
    
    virtual void BeginPlay() override;
};