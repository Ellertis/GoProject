// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardDataAsset.h"
#include "GoTile.h"
#include "Gameplay/Enemies/GoEnemyManager.h"
#include "GoTileManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridGenerated);

UCLASS()
class GOPROJECT_API AGoTileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoTileManager();

	// Components
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
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
	UStaticMesh* LinkMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMeshComponent*> Links;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGoTile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	TSubclassOf<AGoTile> TileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoardDataAsset* DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGoEnemyManager* EnemyManager;

	FOnGridGenerated OnGridGenerated;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TilesSpawner();
	
	int Get1DIndex(int indX, int indY) const; //Convert tile index to place in Tiles array

	FIntPoint Get2DIndex(int TileInd) const;

	FIntPoint GetDeltaIndex(int TileAInd, int TileBInd) const;
	
	bool IsValidIndex(int indX, int indY) const;

	TArray<AGoTile*> GetTilesWithType(ETileType TileType);

	ETileConnection GetOppositeConnections(ETileConnection Dir) const;

	bool AreConnected(int TileIndA, int TileIndB) const;

	TArray<AGoTile*> GetWalkableNeighbors(int TileInd) const;

	void VisualizeConnections();

	void SpawnEnemies();
	
	void LoadGridFromDataAsset(UBoardDataAsset* DataAssetToLoad);

};
