// Fill out your copyright notice in the Description page of Project Settings.

#include "Math/IntPoint.h"
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
	for (ATile* Tile : Tiles){if (IsValid(Tile)){Tile->Destroy(true);}}
	Tiles.Empty();

	FVector Location (FVector(0, 0, 0));
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			Location = FVector(i * Displacement,j * Displacement,0);
			ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileClass,Location,FRotator(0,0,0));
			Tiles.Add(NewTile);
			NewTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void ATileManager::BuildNeighbours()
{
	static const TArray<FIntPoint> Directions =
	{
		FIntPoint(1,0),	//Right
		FIntPoint(-1,0),	//Left
		FIntPoint(0,1),	//Up
		FIntPoint(0,-1)	//Down
	};
	
	Tiles.Reset();
	Tiles.Reserve(4);
	
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			int Index = GetIndex(i,j);
			ATile* Tile = Tiles[Index];
			for(const FIntPoint& Dir : Directions)
			{
				int NX = i + Dir[i];
				int NY = j + Dir[j];
				if(!IsValidIndex(NX,NY))continue;
				Tile->Neighbours.Add(GetIndex(NX,NY));
			}
		}
	}
}

int ATileManager::GetIndex(int indX, int indY) const
{
	return indX + indY * X; //transforms 2d array into 1d array
}

bool ATileManager::IsValidIndex(int indX, int indY) const
{
	return indX >= 0 && indY >= 0 && indX < X && indY < Y;
}

void ATileManager::GenerateGrid()
{
	if (!IsValid(DataAsset)){return;}
	X = DataAsset->X;
	Y = DataAsset->Y;
	Displacement = DataAsset->Displacement;
	TilesSpawner();
}