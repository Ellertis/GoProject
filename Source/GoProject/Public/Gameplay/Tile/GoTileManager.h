// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardDataAsset.h"
#include "GoTile.h"
#include "GoTileManager.generated.h"

UCLASS()
class GOPROJECT_API AGoTileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoTileManager();

	// Components
	UPROPERTY()
	USceneComponent* Root;
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Displacement = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZOffset = 75;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGoTile> LinkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* LinkMesh;

	UPROPERTY()
	TArray<AGoTile*> Links;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGoTile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	TSubclassOf<AGoTile> TileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoardDataAsset* DataAsset;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	void TilesSpawner();

	void BuildNeighbors(); //Assign neighbours indexes to generated tiles
	
	void BuildConnections(); //Store Neighb
	
	int Get1DIndex(int indX, int indY) const; //Convert tile index to place in Tiles array
	
	bool IsValidIndex(int indX, int indY) const;

	AGoTile* GetStartTile();

	ETileConnection GetConnectionsBetween(int TileIndA, int TileIndB) const;
	
	ETileConnection GetOppositeConnections(ETileConnection Dir) const;

	bool AreConnected(int TileIndA, int TileIndB) const;

	TArray<AGoTile*> GetWalkableNeighbors(int TileInd) const;

	void VisualizeConnections();

	void SaveGridToDataAsset(UBoardDataAsset* DataAssetToSave);
	
	void LoadGridFromDataAsset(UBoardDataAsset* DataAssetToLoad);
	
	UFUNCTION(CallInEditor)
	void GenerateGrid();

	UFUNCTION(CallInEditor)
	void UpdateConnections();

	UFUNCTION(CallInEditor)
	void SaveGrid();

	UFUNCTION(CallInEditor)
	void LoadGrid();
	

};
