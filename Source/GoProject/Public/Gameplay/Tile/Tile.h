// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ETileType.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Tile.generated.h"

USTRUCT(BlueprintType)
struct FTileStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;
	
	FTileStruct() : TileType(ETileType::Option1), Mesh(){}
	FTileStruct(ETileType TileType) : TileType(TileType), Mesh(){}
};

UCLASS()
class GOPROJECT_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	// Components
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile")
	FTileStruct TileData;
	
protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
