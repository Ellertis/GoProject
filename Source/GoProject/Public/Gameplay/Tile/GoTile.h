// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoTile.generated.h"
#define ECC_Click ECollisionChannel::ECC_GameTraceChannel1

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Empty UMETA(DisplayName = "Empty"),
	Start UMETA(DisplayName = "Start"),
	End UMETA(DisplayName = "End"),
	Normal UMETA(DisplayName = "Normal"),
	Void UMETA(DisplayName = "Void"),
	Sandwich UMETA(DisplayName = "Sandwich"),
	Jake UMETA(DisplayName = "Jake")
};

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor="true"))
enum class ETileConnection : uint8
{
	None = 0 UMETA(Hidden),
	Xplus    = 1 UMETA(DisplayName="X+"), //0001
	Xminus = 1 << 1 UMETA(DisplayName="X-"), //0010
	Yplus  = 1 << 2 UMETA(DisplayName="Y+"),	//0100
	Yminus  = 1 << 3 UMETA(DisplayName="Y-") //1000
};

UCLASS()
class GOPROJECT_API AGoTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoTile();

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DebugMeshComponent;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* DebugDynamicMaterial;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SandwichRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SandwichOffset = 100;

protected:
	// Called in editor and on spawn
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateDebugColors() const;
	
	void HighLightTile(bool value);

	bool HasConnections(ETileConnection Dir) const;

	void AddConnections(ETileConnection Dir);

	void RemoveConnections(ETileConnection Dir);

	void ClearConnections();

};
