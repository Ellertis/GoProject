// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Tile/GoTileManager.h"

#include "Core/GoGameModeBase.h"
#include "Math/IntPoint.h"

static const TArray<TPair<FIntPoint, ETileConnection>> Directions =
{
	{ FIntPoint(1,0), ETileConnection::Right },
	{ FIntPoint(-1,0), ETileConnection::Left },
	{ FIntPoint(0,1), ETileConnection::Up },
	{ FIntPoint(0,-1), ETileConnection::Down }
};

// Sets default values
AGoTileManager::AGoTileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
}

void AGoTileManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
}

// Called when the game starts or when spawned
void AGoTileManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGoTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoTileManager::TilesSpawner()
{
	if (Tiles.Num() != 0){
		for (AGoTile* Tile : Tiles){if(IsValid(Tile))Tile->Destroy(true);}
	}
	Tiles.Empty();
	Tiles.SetNum(X*Y);

	FVector Location (FVector(0, 0, 0));
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			int Index = Get1DIndex(i, j);
			Location = FVector(i * Displacement,j * Displacement,0);
			AGoTile* NewTile = GetWorld()->SpawnActor<AGoTile>(TileClass,Location,FRotator(0,0,0));
			Tiles[Index] = NewTile;
			NewTile->Index = Index;
			NewTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void AGoTileManager::BuildNeighbors()
{
	for (int i=0; i<X; i++)
	{
		for (int j=0; j<Y; j++)
		{
			if(!IsValidIndex(i,j)) continue;
			int Index = Get1DIndex(i,j); // transform 2d coords to 1d array
			AGoTile* Tile = Tiles[Index];
			Tile->Neighbors.Empty();
			
			for(const TPair<FIntPoint, ETileConnection>& Dir : Directions)
			{
				int NX = i + Dir.Key.X;
				int NY = j + Dir.Key.Y;
				if (!IsValidIndex(NX, NY)) continue;
				int NeighborIndex = Get1DIndex(NX, NY);
				Tile->Neighbors.Add(NeighborIndex);
			}
		}
	}
}

void AGoTileManager::BuildConnections()
{
	for (AGoTile* Tile : Tiles)
	{
		if (!IsValid(Tile)) continue;
		//Tile->ClearConnections();
		
		for (int Neighbor : Tile->Neighbors)
		{
			if(AreConnected(Tile->Index, Neighbor)) continue;
			AGoTile* NeighborTile = Tiles[Neighbor];
			if (!NeighborTile) continue;

			int AX = Tile->Index % X, AY = Tile->Index / X,
			BX = NeighborTile->Index % X, BY = NeighborTile->Index / X; // convert 1d array to 2d array

			FIntPoint DeltaDirection(BX - AX, BY - AY);
			for (const TPair<FIntPoint, ETileConnection>& Dir : Directions)
			{
				if (Dir.Key != DeltaDirection) continue;
				Tile->AddConnections(Dir.Value);
				NeighborTile->AddConnections(GetOppositeConnections(Dir.Value));
			}
		}
	}
}

int AGoTileManager::Get1DIndex(int indX, int indY) const
{
	return indX + indY * X; //transforms 2d array into 1d array
}

bool AGoTileManager::IsValidIndex(int indX, int indY) const
{
	return indX >= 0 && indY >= 0 && indX < X && indY < Y;
}

ETileConnection AGoTileManager::GetConnectionsBetween(int TileIndA, int TileIndB) const
{
    if (!Tiles.IsValidIndex(TileIndA) || !Tiles.IsValidIndex(TileIndB)) return ETileConnection::None;
	AGoTile* TileA = Tiles[TileIndA];
    AGoTile* TileB = Tiles[TileIndB];

    int AX = TileIndA % X, AY = TileIndA / X, BX = TileIndB % X, BY = TileIndB / X; // convert 1d array to 2d array

    FIntPoint DeltaDirection(BX - AX, BY - AY);
	ETileConnection Result = ETileConnection::None;
	
    for (const TPair<FIntPoint, ETileConnection>& Dir : Directions)
    {
    	if (Dir.Key != DeltaDirection) continue;

    	if (TileA->HasConnections(Dir.Value))
    		Result = static_cast<ETileConnection>(
    		static_cast<int>(Result) | static_cast<int>(Dir.Value));
    	
    	if (TileB->HasConnections(GetOppositeConnections(Dir.Value)))
    		Result = static_cast<ETileConnection>(
    		static_cast<int>(Result) | static_cast<int>(GetOppositeConnections(Dir.Value)));
    }
    return Result;
}


ETileConnection AGoTileManager::GetOppositeConnections(ETileConnection Dir) const
{
	switch (Dir)
	{
	case ETileConnection::Up: return ETileConnection::Down;
	case ETileConnection::Down: return ETileConnection::Up;
	case ETileConnection::Right: return ETileConnection::Left;
	case ETileConnection::Left: return ETileConnection::Right;
	case ETileConnection::None: return ETileConnection::None;
	default: return ETileConnection::None;
	}
}

bool AGoTileManager::AreConnected(int TileIndA, int TileIndB) const
{
	const AGoTile* TileA = Tiles.IsValidIndex(TileIndA) ? Tiles[TileIndA] : nullptr;
	const AGoTile* TileB = Tiles.IsValidIndex(TileIndB) ? Tiles[TileIndB] : nullptr;
	if (!TileA || !TileB) return false;
	
	int AX = TileIndA % X, AY = TileIndA / X, BX = TileIndB % X, BY = TileIndB / X; // convert 1d array to 2d array

	FIntPoint DeltaDirection(BX - AX, BY - AY);
	
	for(const TPair<FIntPoint, ETileConnection>& Dir : Directions)
	{
		if (Dir.Key != DeltaDirection) continue;
		return TileA->HasConnections(Dir.Value) && TileB->HasConnections(GetOppositeConnections(Dir.Value));
	}
	return false;
}

TArray<AGoTile*> AGoTileManager::GetWalkableNeighbors(int TileInd) const
{
	TArray<AGoTile*> Result;
	if (!Tiles.IsValidIndex(TileInd)) return Result;
	AGoTile* Tile = Tiles[TileInd];

	for (int NeighborIndex : Tile->Neighbors)
	{
		if (!Tiles.IsValidIndex(NeighborIndex)) continue;
		AGoTile* Neighbor = Tiles[NeighborIndex];
		if (Neighbor && Neighbor->Walkable && AreConnected(TileInd, NeighborIndex)){Result.Add(Neighbor);}
	}
	return Result;
}

void AGoTileManager::VisualizeConnections()
{
	for (AGoTile* Link : Links){if(IsValid(Link))Link->Destroy(true);}
	Links.Empty();

	for (int i=0;i<Tiles.Num();i++)
	{
		AGoTile* Tile = Tiles[i];

		for (int NeighborIndex : Tile->Neighbors)
		{
			if (NeighborIndex < i) continue;
			if (!AreConnected(i, NeighborIndex)) continue;
			AGoTile* NeighbourTile = Tiles[NeighborIndex];
			if (!NeighbourTile) continue;

			FVector Start = Tile->GetActorLocation();
			FVector End = NeighbourTile->GetActorLocation();
			FVector Dir = End - Start;
			float Length = Dir.Size();

			// Spawn Links
			FTransform LinkTransform;
			LinkTransform.SetLocation(Start + Dir / 2 + FVector(0,0,ZOffset)); // midpoint
			LinkTransform.SetRotation(FQuat::FindBetweenNormals(FVector::UpVector, Dir.GetSafeNormal()));
			LinkTransform.SetScale3D(FVector(0.1f, 0.1f, Length / 100));

			AGoTile* Link = GetWorld()->SpawnActor<AGoTile>(LinkClass,LinkTransform);
			Link->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
			Links.Add(Link);
			if (LinkMesh){Link->MeshComponent->SetStaticMesh(LinkMesh);}
		}
	}
}

void AGoTileManager::SaveGridToDataAsset(UBoardDataAsset* DataAssetToSave)
{
	if (!IsValid(DataAssetToSave)) return;

	DataAssetToSave->X = X;
	DataAssetToSave->Y = Y;
	DataAssetToSave->Displacement = Displacement;
	DataAssetToSave->Tiles.Empty();
	DataAssetToSave->Tiles.SetNum(Tiles.Num());

	for (int i = 0; i < Tiles.Num(); i++)
	{
		AGoTile* Tile = Tiles[i];
		if (!IsValid(Tile)) continue;

		FGoTileData& TileData = DataAsset->Tiles[i];
		TileData.TileClass = Tile->GetClass();
		TileData.Connections = Tile->Connections;
		TileData.TileType = Tile->TileType;
		TileData.Walkable = Tile->Walkable;
	}

#if WITH_EDITOR
	DataAsset->MarkPackageDirty();
#endif
}

void AGoTileManager::LoadGridFromDataAsset(UBoardDataAsset* DataAssetToLoad)
{
	if (!IsValid(DataAssetToLoad)) return;

	X = DataAssetToLoad->X;
	Y = DataAssetToLoad->Y;
	Displacement = DataAssetToLoad->Displacement;

	TilesSpawner();
	
	for (int i = 0; i < Tiles.Num(); i++)
	{
		if (!Tiles.IsValidIndex(i)) continue;
		AGoTile* Tile = Tiles[i];
		if (!IsValid(Tile)) continue;

		if (DataAssetToLoad->Tiles.Num() <= i) break;
		const FGoTileData& TileData = DataAssetToLoad->Tiles[i];
		Tile->Connections = TileData.Connections;
		Tile->TileType = TileData.TileType;
		Tile->Walkable = TileData.Walkable;
	}

	BuildNeighbors();
	VisualizeConnections();
}


void AGoTileManager::GenerateGrid()
{
	if (!IsValid(DataAsset)) return;
	X = DataAsset->X;
	Y = DataAsset->Y;
	Displacement = DataAsset->Displacement;
	TilesSpawner();
	BuildNeighbors();
	BuildConnections();
	VisualizeConnections();
}

void AGoTileManager::UpdateConnections()
{
	VisualizeConnections();
}

void AGoTileManager::SaveGrid()
{
	if (!IsValid(DataAsset)) return;
	SaveGridToDataAsset(DataAsset);
}

void AGoTileManager::LoadGrid()
{
	if (!IsValid(DataAsset)) return;
	LoadGridFromDataAsset(DataAsset);
}

