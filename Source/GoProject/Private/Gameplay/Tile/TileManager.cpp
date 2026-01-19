// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Tile/TileManager.h"

// Sets default values
ATileManager::ATileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ATileManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
/*
	if (!TileClass) return;
	
	for (ATile* Tile : Tiles)
	{
		if (IsValid(Tile))
		{
			Tile->Destroy();
		}
	}
	Tiles.Empty();
	
	TilesSpawner();
	*/
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

void ATileManager::SpawnTile(const FVector& Location)
{
	ATile* NewTile = GetWorld()->SpawnActor<ATile>(Location,FRotator(0,0,0));
	Tiles.Add(NewTile);
	NewTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
}

void ATileManager::TilesSpawner()
{
	FVector TileLocation = FVector(0, 0, 0);
	for (uint8 i=0; i<=X; i++)
	{
		for (uint8 j=0; j<=Y; j++)
		{
			TileLocation = FVector(
			TileLocation.X * Displacement,
			TileLocation.Y * Displacement,
			0);
			SpawnTile(TileLocation);
		}
	}
}