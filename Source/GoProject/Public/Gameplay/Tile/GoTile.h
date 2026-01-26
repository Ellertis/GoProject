// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoTile.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Option1 UMETA(DisplayName = "Option1"),
	Option2 UMETA(DisplayName = "Option2"),
	Empty UMETA(DisplayName = "Empty"),
	Start UMETA(DisplayName = "Start"),
	End UMETA(DisplayName = "End")
};

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ETileConnection : uint8
{
	None = 0 UMETA(DisplayName = "Hidden"),
	Up    = 1, //0001
	Right = 1 << 1, //0010
	Down  = 1 << 2,	//0100
	Left  = 1 << 3 //1000
};

UCLASS()
class GOPROJECT_API AGoTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoTile();

	// Components
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY()
	UStaticMeshComponent* MeshComponent;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Neighbors;

	UPROPERTY(BlueprintReadWrite)
	int Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType = ETileType::Empty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/GoProject.ETileConnection"))
	int32 Connections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Walkable = true;


protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HighLightTile(bool value);

	bool HasConnections(ETileConnection Dir) const;

	void AddConnections(ETileConnection Dir);

	void RemoveConnections(ETileConnection Dir);

	void ClearConnections();

};
