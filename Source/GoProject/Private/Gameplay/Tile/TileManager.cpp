// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InstancedStaticMeshComponent.h"
#include "Gameplay/Tile/TileManager.h"
#include "Gameplay/Tile/ETileType.h"
#include "Gameplay/Tile/TileStruct.h"

// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BasicTiles = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BasicTiles"));
	RootComponent = BasicTiles;
	
	EmptyTiles = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("EmptyTiles"));
}

void ATileManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	Tiles.SetNum(X*Y);
	
	BasicTiles->SetStaticMesh(BasicStaticMesh);
	BasicTiles->ClearInstances();
	TilesSpawner();
	
}

// Called when the game starts or when spawned
void ATileManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileManager::TilesSpawner()
{
	//FVector TileLocation (FVector(0, 0, 0));
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			FTransform T(FVector(i * Displacement,j * Displacement,0));
			BasicTiles->AddInstance(T);
			
			int Index = GetIndex(i,j);
			Tiles[Index].TileType = ETileType::Option1; 
		}
	}
}

int ATileManager::GetIndex(int indX, int indY) const
{
	return indX + indY * X; //transforms 2d array into 1d array
}
