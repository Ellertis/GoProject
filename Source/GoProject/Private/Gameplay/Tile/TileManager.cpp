// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Tile/TileManager.h"

#include "Kismet/GameplayStatics.h"
#include "Math/IntPoint.h"

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

ATileManager* ATileManager::Get(UWorld* World)
{
	return Cast<ATileManager>(UGameplayStatics::GetActorOfClass(World, ATileManager::StaticClass()));
}

void ATileManager::TilesSpawner()
{
	for (ATile* Tile : Tiles){if(IsValid(Tile))Tile->Destroy(true);}
	Tiles.Empty();
	Tiles.SetNum(X*Y);

	FVector Location (FVector(0, 0, 0));
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			int Index = GetIndex(i, j);
			Location = FVector(i * Displacement,j * Displacement,0);
			if (TempLDTiles.Num() > 0)
			{
				ATile* PreDefinedTile = GetWorld()->SpawnActor<ATile>(TempLDTiles[GetIndex(i,j)],Location,FRotator(0,0,0));
				Tiles[Index] = PreDefinedTile;
				PreDefinedTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform); continue;
			}
			ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileClass,Location,FRotator(0,0,0));
			Tiles[Index] = NewTile;
			NewTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void ATileManager::BuildNeighbors()
{
	static const TArray<FIntPoint> Directions =
	{
		FIntPoint(1,0),	//R
		FIntPoint(-1,0),	//L
		FIntPoint(0,1),	//U
		FIntPoint(0,-1)	//D
	};
	
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			int Index = GetIndex(i,j); // transform 2d coords to 1d array
			ATile* Tile = Tiles[Index];
			if (!Tile || !Tile->Walkable) continue;
			Tile->Neighbors.Empty();
			for(const FIntPoint& Dir : Directions)
			{
				int NX = i + Dir.X;
				int NY = j + Dir.Y;
				if(!IsValidIndex(NX,NY))continue;
				int NeighbourIndex = GetIndex(NX,NY);
				ATile* NeighbourTile = Tiles[NeighbourIndex];
				if (!NeighbourTile || !NeighbourTile->Walkable) continue;
				Tile->Neighbors.Add(GetIndex(NX,NY));
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

TArray<ATile*> ATileManager::GetWalkableNeighbors(ATile* Tile) const
{
	TArray<ATile*> Result;
	if (!Tile) return Result;

	for (int Index : Tile->Neighbors)
	{
		if (Tiles.IsValidIndex(Index))
		{
			ATile* Neighbor = Tiles[Index];
			if (Neighbor && Neighbor->Walkable)
				Result.Add(Neighbor);
		}
	}
	return Result;
}

void ATileManager::VisualizeGraph()
{
	for (ATile* Link : Links){if(IsValid(Link))Link->Destroy(true);}
	Links.Empty();
	
	for (int i=0;i<Tiles.Num();i++)
	{
		ATile* Tile = Tiles[i];
		FVector Start = Tile->GetActorLocation();

		for (int NeighborIndex : Tile->Neighbors)
		{
			if (NeighborIndex < i) continue;
			ATile* NeighbourTile = Tiles[NeighborIndex];
			if (!NeighbourTile || !NeighbourTile->Walkable) continue;

			FVector End = NeighbourTile->GetActorLocation();
			FVector Dir = End - Start;
			float Length = Dir.Size();

			// Spawn cylinder
			FTransform CylinderTransform;
			CylinderTransform.SetLocation(Start + Dir / 2 + FVector(0,0,ZOffset)); // midpoint
			CylinderTransform.SetRotation(FQuat::FindBetweenNormals(FVector::UpVector, Dir.GetSafeNormal()));
			CylinderTransform.SetScale3D(FVector(0.1f, 0.1f, Length / 100)); // adjust radius & height

			ATile* Link = GetWorld()->SpawnActor<ATile>(LinkClass,CylinderTransform);
			Link->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
			Links.Add(Link);
			if (Link)
			{
				if (LinkMesh){
					Link->MeshComponent->SetStaticMesh(LinkMesh); // assign in editor
				}
			}
		}
	}
}


void ATileManager::GenerateGrid()
{
	if (!IsValid(DataAsset)){return;}
	X = DataAsset->X;
	Y = DataAsset->Y;
	Displacement = DataAsset->Displacement;
	TilesSpawner();
	BuildNeighbors();
	VisualizeGraph();
}