// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "TileManager.generated.h"

UCLASS()
class GOPROJECT_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

	// Variables
	UPROPERTY(BlueprintReadWrite)
	uint8 X = 2;

	UPROPERTY(BlueprintReadWrite)
	uint8 Y = 2;

	UPROPERTY(BlueprintReadWrite)
	uint8 Displacement = 1;

	UPROPERTY(BlueprintReadWrite)
	TArray<ATile*> Tiles;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<ATile> TileClass;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SpawnTile(const FVector &InLocation);
	void TilesSpawner();
};
