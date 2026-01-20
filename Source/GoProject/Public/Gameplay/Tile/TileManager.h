// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileDataAsset.h"
#include "Tile.h"
#include "TileManager.generated.h"

UCLASS()
class GOPROJECT_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

	// Components
	UPROPERTY()
	USceneComponent* Root;
	
	// Variables
	UPROPERTY()
	int X = 2;

	UPROPERTY()
	int Y = 2;

	UPROPERTY()
	int Displacement = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ATile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	TSubclassOf<ATile> TileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTileDataAsset* DataAsset;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void TilesSpawner();

	void BuildNeighbours(); //Assign neighbours indexes to generated tiles
	
	int GetIndex(int indX, int indY) const;
	
	bool IsValidIndex(int indX, int indY) const;
	
	UFUNCTION(CallInEditor)
	void GenerateGrid();
};
