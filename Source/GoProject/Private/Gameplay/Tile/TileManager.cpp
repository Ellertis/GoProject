// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Tile/TileManager.h"

// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
}

void ATileManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

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
	Tiles.SetNum(X*Y);
	
	for (ATile* Tile : Tiles){
		if (IsValid(Tile)){Tile->Destroy(true);}
	}

	Tiles.Empty();

	FVector Location (FVector(0, 0, 0));
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			Location = FVector(i * Displacement,j * Displacement,0);
			ATile* NewTile = GetWorld()->SpawnActor<ATile>(Location,FRotator(0,0,0));
			Tiles.Add(NewTile);
			NewTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

int ATileManager::GetIndex(int indX, int indY) const
{
	return indX + indY * X; //transforms 2d array into 1d array
}

void ATileManager::GenerateGrid()
{
	TilesSpawner();
}