// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GoPawn.h"
#include "GoPawnPlayer.generated.h"

class AGoPlayerController;
class UInputMappingContext;
class UInputAction;

#define ECC_Player ECollisionChannel::ECC_GameTraceChannel2

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerMovement);

UCLASS(Blueprintable)
class GOPROJECT_API AGoPawnPlayer : public AGoPawn
{
    GENERATED_BODY()

public:
    AGoPawnPlayer();

    UPROPERTY(BlueprintReadWrite, Category = "References")
    AGoPlayerController* PlayerController;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ClickAction;
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JakePlacementAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jake")
    TSubclassOf<AGoTile> JakeTileClass;
    
    UPROPERTY(BlueprintReadOnly, Category = "Jake")
    AGoTile* CurrentJakeTile;
    
    UPROPERTY(BlueprintReadOnly, Category = "Jake")
    AGoTile* OriginalVoidTile;

    UPROPERTY(BlueprintReadOnly, Category = "Jake")
    bool bIsJakePlacementMode = false;

    UPROPERTY(BlueprintReadOnly, Category = "Jake")
    TArray<AGoTile*> HighlightedJakeTiles;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPlayerMovement OnPlayerMovement;
	
	UPROPERTY(BlueprintReadOnly, Category = "Jake")
	TSet<AGoPawn*> EntitiesOnJakeTile;

	UPROPERTY(BlueprintReadOnly, Category = "Jake")
	bool JakeIsPlaced = false;

	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnDamageTaken();
	virtual void OnDamageTaken_Implementation();
	
	UFUNCTION(BlueprintCallable)
	void RegisterEntityOnJakeTile(AGoPawn* Entity);

	UFUNCTION(BlueprintCallable)
	void UnregisterEntityOnJakeTile(AGoPawn* Entity);

	UFUNCTION(BlueprintCallable)
	void CheckJakeTileRemoval();
	
    UFUNCTION(BlueprintNativeEvent, Category = "Effects")
    void OnCollectSandwich();
    virtual void OnCollectSandwich_Implementation();
    
    UFUNCTION(BlueprintNativeEvent, Category = "Effects")
    void OnPlaceJakeTile();
    virtual void OnPlaceJakeTile_Implementation();
    
    UFUNCTION(BlueprintNativeEvent, Category = "Effects")
    void OnRemoveJakeTile();
    virtual void OnRemoveJakeTile_Implementation();
	
    UFUNCTION(BlueprintNativeEvent, Category = "Effects")
    void OnEnterJakePlacementMode();
    virtual void OnEnterJakePlacementMode_Implementation();
    
    UFUNCTION(BlueprintNativeEvent, Category = "Effects")
    void OnExitJakePlacementMode();
    virtual void OnExitJakePlacementMode_Implementation();

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

    UFUNCTION(BlueprintCallable)
    void PlacePlayer(AGoTile* Tile);
    
    UFUNCTION()
    void OnClickTrigger();
    
    UFUNCTION()
    void OnClickReleased();
	
    UFUNCTION()
    void OnJakePlacementTriggered();
    
    virtual TArray<AGoTile*> GetValidMoveTiles() const override;
    
    UFUNCTION(BlueprintCallable)
    void MoveToTile(AGoTile* Tile);
    
    void ToggleHighlightNeighbors(bool value) const;
    void FinishTurn() const;

    UFUNCTION(BlueprintCallable)
    void CollectSandwich();
	
    UFUNCTION(BlueprintCallable)
    void ToggleJakePlacementMode();

    UFUNCTION(BlueprintCallable)
    void EnterJakePlacementMode();

    UFUNCTION(BlueprintCallable)
    void ExitJakePlacementMode();
    
    UFUNCTION(BlueprintCallable)
    bool TryPlaceJakeTile(AGoTile* TargetVoidTile);
    
    UFUNCTION(BlueprintCallable)
    void RemoveJakeTile();
    
    UFUNCTION(BlueprintCallable)
    TArray<AGoTile*> GetValidJakePlacementTiles() const;

protected:
    UPROPERTY()
    AActor* SelectedActor;
    
    void OnJakeTileExited();
	
	void UpdateJakePlacementHighlights();

	void ClearJakePlacementHighlights();
};