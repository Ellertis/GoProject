// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Tile/GoTileManager.h"
#include "Math/IntPoint.h"

static const TArray<TPair<FIntPoint, ETileConnection>> Directions =
{
	{FIntPoint(1,0), ETileConnection::Xplus},
	{FIntPoint(-1,0), ETileConnection::Xminus},
	{FIntPoint(0,1), ETileConnection::Yplus},
	{FIntPoint(0,-1), ETileConnection::Yminus}
};



// Sets default values
AGoTileManager::AGoTileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	RootComponent->SetMobility(EComponentMobility::Static);
	
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
	if (X <= 0 || Y <= 0){UE_LOG(LogTemp, Warning, TEXT("TileManager: X or Y is set to 0"));}
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
			AGoTile* NewTile = GetWorld()->SpawnActor<AGoTile>(DataAsset->Tiles[Index].TileClass,
				Location,
				FRotator(0,0,0));
			Tiles[Index] = NewTile;
			NewTile->Index = Index;
			NewTile->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

int AGoTileManager::Get1DIndex(int indX, int indY) const
{
	return indX + indY * X; // transforms 2d array into 1d array
}

FIntPoint AGoTileManager::Get2DIndex(int TileInd) const
{
	return FIntPoint(TileInd % X, TileInd / X); // get 2d coordinate of a tile from its index
}

FIntPoint AGoTileManager::GetDeltaIndex(int TileAInd, int TileBInd) const
{
	FIntPoint TileACoord = Get2DIndex(TileAInd), TileBCoord = Get2DIndex(TileBInd);
	return FIntPoint(TileBCoord.X - TileACoord.X, TileBCoord.Y - TileACoord.Y);
}

bool AGoTileManager::IsValidIndex(int indX, int indY) const
{
	return indX >= 0 && indY >= 0 && indX < X && indY < Y; // is tile in the grid
}

TArray<AGoTile*> AGoTileManager::GetTilesWithType(ETileType TileType)
{
	TArray<AGoTile*> Result;
	for (AGoTile* Tile : Tiles)
	{
		if (IsValid(Tile) && Tile->TileType == TileType) Result.Add(Tile);
	}
	return Result;
}

ETileConnection AGoTileManager::GetOppositeConnections(ETileConnection Dir) const
{
	switch (Dir)
	{
	case ETileConnection::Xplus: return ETileConnection::Xminus;
	case ETileConnection::Xminus: return ETileConnection::Xplus;
	case ETileConnection::Yplus: return ETileConnection::Yminus;
	case ETileConnection::Yminus: return ETileConnection::Yplus;
	case ETileConnection::None: return ETileConnection::None;
	default: return ETileConnection::None;
	}
}

bool AGoTileManager::AreConnected(int TileIndA, int TileIndB) const
{
	const AGoTile* TileA = Tiles.IsValidIndex(TileIndA) ? Tiles[TileIndA] : nullptr;
	const AGoTile* TileB = Tiles.IsValidIndex(TileIndB) ? Tiles[TileIndB] : nullptr;
	if (!TileA || !TileB) return false;
	
	FIntPoint DeltaDirection(GetDeltaIndex(TileIndA,TileIndB));
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
	if(Links.Num() != 0){
		for (UStaticMeshComponent* Link : Links){if(IsValid(Link))Link->DestroyComponent();}
	}
	Links.Empty();
	
	for (int i=0;i<Tiles.Num();i++)
	{
		AGoTile* Tile = Tiles[i];

		if (Tile->TileType == ETileType::Void) {continue;}
		
		for (int NeighborIndex : Tile->Neighbors)
		{
			if (NeighborIndex < i) continue;
			if (!AreConnected(i, NeighborIndex)) continue;

			AGoTile* NeighbourTile = Tiles[NeighborIndex];
			if (!NeighbourTile) continue;

			if (NeighbourTile->TileType == ETileType::Void) {continue;}
			
			FVector Start = Tile->GetActorLocation()+LinkOffset, End = NeighbourTile->GetActorLocation()+LinkOffset;
			FVector Dir = End - Start;
			float Length = Dir.Size();

			// Spawn Links
			FTransform LinkTransform;
			LinkTransform.SetLocation(Start + Dir / 2 + FVector(0,0,ZOffset)); // midpoint + offset
			//LinkTransform.SetRotation(UE::Math::TQuat<double>(FRotationMatrix::MakeFromZ(Dir).Rotator())); // alt rotation calculation method
			//LinkTransform.SetRotation(FQuat::FindBetweenNormals(FVector::UpVector, Dir.GetSafeNormal()));
			FRotator Rot = Dir.Rotation();
			LinkTransform.SetRotation(Rot.Quaternion());
			LinkTransform.SetScale3D(FVector(LinkScale.X, LinkScale.Y, Length / 100));
			
			FName ComponentName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(),TEXT("Link"));
			UStaticMeshComponent* ConnectionComponent = NewObject<UStaticMeshComponent>(this,ComponentName);
			ConnectionComponent->SetupAttachment(GetRootComponent());
			ConnectionComponent->RegisterComponent();
			ConnectionComponent->SetWorldTransform(LinkTransform);
			Links.Add(ConnectionComponent);
			if(!LinkMesh) continue;
			ConnectionComponent->SetStaticMesh(LinkMesh);

		}
	}
}

void AGoTileManager::SpawnEnemies()
{
	if(!IsValid(EnemyManager)) return;
	for(int i=0;i<Tiles.Num();i++)
	{
		FGoTileData& TileData = DataAsset->Tiles[i];
		if(TileData.Enemies.Num() <= 0) continue;
		AGoTile* Tile = Tiles[i];
		for(const FEnemySpawnData& SpawnData : TileData.Enemies)
		{
			for (int k = 0; k < SpawnData.Count; k++)
			{
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(Tile->GetActorLocation() + FVector(0,0,ZOffset+ZOffset));

				FRotator SpawnRotator;
				switch(SpawnData.FaceDirection)
				{
					case EFaceDirection::Xplus: SpawnRotator = GetActorForwardVector().Rotation();break;
					case EFaceDirection::Xminus: SpawnRotator = (-GetActorForwardVector()).Rotation();break;
					case EFaceDirection::Yplus: SpawnRotator = GetActorRightVector().Rotation();break;
					case EFaceDirection::Yminus: SpawnRotator = (-GetActorRightVector()).Rotation();break;
					default: SpawnRotator = FRotator::ZeroRotator;break;
				}
				SpawnTransform.SetRotation(SpawnRotator.Quaternion());
				
				EnemyManager->SpawnEnemy(SpawnTransform,SpawnData.EnemyClass,Tiles[i],SpawnData.FaceDirection);
			}
		}
	}
}

void AGoTileManager::LoadGridFromDataAsset(UBoardDataAsset* DataAssetToLoad)
{
	if (!IsValid(DataAssetToLoad)) return;
	DataAsset = DataAssetToLoad;
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
		Tile->Neighbors = TileData.Neighbors;
		Tile->TileType = TileData.TileType;
		Tile->Walkable = TileData.Walkable;
		if (Tile->TileType == ETileType::Void) {Tile->Walkable = false;}
		Tile->UpdateDebugColors();
	}
	
	VisualizeConnections();
	OnGridGenerated.Broadcast();
	UE_LOG(LogTemp, Display, TEXT("Broadcast OnGridGenerated"));
	SpawnEnemies();
}

