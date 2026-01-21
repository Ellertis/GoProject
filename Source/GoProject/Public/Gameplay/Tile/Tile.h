// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Option1 UMETA(DisplayName = "Option1"),
	Option2 UMETA(DisplayName = "Option2"),
	Empty UMETA(DisplayName = "Empty")
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
	UStaticMeshComponent* MeshComponent;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Neighbors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType = ETileType::Empty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Walkable = false;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
