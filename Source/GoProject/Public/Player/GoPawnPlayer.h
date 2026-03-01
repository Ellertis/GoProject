// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Core/GoPawn.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "GoPawnPlayer.generated.h"

#define  ECC_Click ECollisionChannel::ECC_GameTraceChannel1

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerMovement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

UCLASS()
class GOPROJECT_API AGoPawnPlayer : public AGoPawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	AGoPawnPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	UInputAction* ClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Input")
	UInputAction* TouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Ability")
	bool bIsAlive = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Ability")
	bool bIsMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Ability")
	bool bCanClickTile = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player|Tile")
	AGoTile* TargetTile;
	
	UPROPERTY()
	AActor* SelectedActor;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AGoTileManager* TM;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called on possession
	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlacePlayer(AGoTile* Tile);
	
	UPROPERTY()
	AGoTurnManager* TurnManager;

	FOnPlayerMovement OnPlayerMovement;
	FOnPlayerDeath OnPlayerDeath;

private:
	void OnClickTrigger();
	
	void OnClickReleased();
	
	TArray<AGoTile*> GetValidMoveTiles() const;

	void MoveToTile(AGoTile* Tile);

	void ToggleHighlightNeighbors(bool value) const;

	void  FinishTurn() const;
	
};