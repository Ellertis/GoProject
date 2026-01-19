// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileStruct.h"
#include "TileManager.generated.h"

UCLASS()
class GOPROJECT_API ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

	// Components
	UPROPERTY(BlueprintReadWrite)
	UInstancedStaticMeshComponent* BasicTiles;

	UPROPERTY(BlueprintReadWrite)
	UInstancedStaticMeshComponent* EmptyTiles;

	UPROPERTY(BlueprintReadWrite)
	UInstancedStaticMeshComponent* MoreTiles;
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Displacement = 100;

	UPROPERTY()
	TArray<FTileStruct> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* BasicStaticMesh;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void TilesSpawner();
	int GetIndex(int indX, int indY) const;
};
