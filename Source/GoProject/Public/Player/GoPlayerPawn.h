// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "GoPlayerPawn.generated.h"

UCLASS()
class GOPROJECT_API AGoPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoPlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Components
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* ClickAction;

	UPROPERTY()
	AActor* SelectedActor;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AGoTileManager* TM;
	
	UPROPERTY()
	AGoTile* CurrTile;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called on possession
	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void OnClickTrigger();
	void OnClickReleased();

	TArray<AGoTile*> GetValidMoveTiles() const;

	void MoveToTile(AGoTile* Tile);

	void ToggleHighlightNeighbors(bool value) const;
	
};
