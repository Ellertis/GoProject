// Fill out your copyright notice in the Description page of Project Settings.


#include "GoBoardEditorSubsystem.h"


#include "Editor.h"
#include "Engine/World.h"
#include "Math/IntPoint.h"

static const TArray<TPair<FIntPoint, ETileConnection>> Directions =
{
	{FIntPoint(1,0), ETileConnection::Xplus},
	{FIntPoint(-1,0), ETileConnection::Xminus},
	{FIntPoint(0,1), ETileConnection::Yplus},
	{FIntPoint(0,-1), ETileConnection::Yminus}
};

UWorld* UGoBoardEditorSubsystem::GetEditorWorld() const
{
	if (!GEditor) return nullptr;
	return GEditor->GetEditorWorldContext().World();
}

void UGoBoardEditorSubsystem::ClearPreview()
{
	for (AGoTile* Tile : PreviewTiles)
	{
		if (IsValid(Tile))
		{
			Tile->Destroy();
		}
	}
	PreviewTiles.Empty();
}

void UGoBoardEditorSubsystem::ClearVisualizationConnections()
{
	for(UStaticMeshComponent* Link : Links)
	{
		if (IsValid(Link))
		{
			Link->DestroyComponent();
		}
	}
	Links.Empty();
}

void UGoBoardEditorSubsystem::BuildNeighbors()
{
	for (int i=0; i<GridX; i++)
	{
		for (int j=0; j<GridY; j++)
		{
			if (i < 0 || j < 0 || i >= GridX || j >= GridY) continue;
			int Index = Get1DIndex(i,j); // transform 2d coords to 1d array
			AGoTile* Tile = PreviewTiles[Index];
			Tile->Neighbors.Empty();
			
			for(const TPair<FIntPoint, ETileConnection>& Dir : Directions)
			{
				int NX = i + Dir.Key.X;
				int NY = j + Dir.Key.Y;
				if (NX < 0 || NY < 0 || NX >= GridX || NY >= GridY) continue;
				int NeighborIndex = Get1DIndex(NX, NY);
				Tile->Neighbors.Add(NeighborIndex);
			}
		}
	}
}

void UGoBoardEditorSubsystem::BuildConnections()
{
	for (AGoTile* Tile : PreviewTiles)
	{
		if (!IsValid(Tile)) continue;
		
		for (int NeighborIndex : Tile->Neighbors)
		{
			if (NeighborIndex < Tile->Index) continue;
			AGoTile* NeighborTile = PreviewTiles[NeighborIndex];
			if (!NeighborTile) continue;
			
			FIntPoint DeltaDirection(GetDeltaIndex(Tile->Index,NeighborIndex));
			for (const TPair<FIntPoint, ETileConnection>& Dir : Directions)
			{
				if (Dir.Key != DeltaDirection) continue;
				Tile->AddConnections(Dir.Value);
				NeighborTile->AddConnections(TileManager->GetOppositeConnections(Dir.Value));
				break;
			}
		}
	}
}

bool UGoBoardEditorSubsystem::AreConnected(int TileIndA, int TileIndB) const
{
	const AGoTile* TileA = PreviewTiles[TileIndA];
	const AGoTile* TileB = PreviewTiles[TileIndB];
	if (!TileA || !TileB) return false;
	
	FIntPoint DeltaDirection(GetDeltaIndex(TileIndA,TileIndB));
	for(const TPair<FIntPoint, ETileConnection>& Dir : Directions)
	{
		if (Dir.Key != DeltaDirection) continue;
		return TileA->HasConnections(Dir.Value) && TileB->HasConnections(TileManager->GetOppositeConnections(Dir.Value));
	}
	return false;
}

FIntPoint UGoBoardEditorSubsystem::GetDeltaIndex(int TileAInd, int TileBInd) const
{
	FIntPoint TileACoord = Get2DIndex(TileAInd), TileBCoord = Get2DIndex(TileBInd);
	return FIntPoint(TileBCoord.X - TileACoord.X, TileBCoord.Y - TileACoord.Y);
}

FIntPoint UGoBoardEditorSubsystem::Get2DIndex(int TileInd) const
{
	return FIntPoint(TileInd % GridX, TileInd / GridX);
}

int UGoBoardEditorSubsystem::Get1DIndex(int TileX, int TileY) const
{
	return TileX + TileY * GridX;
}

void UGoBoardEditorSubsystem::GenerateGrid(int X,int Y,int Displacement)
{
	if (!TileManager || X <= 0 || Y <= 0 ) return;
	GridX = X;
	GridY = Y;
	GridDisplacement = Displacement;
	
	UWorld* World = GetEditorWorld();
	if (!World || !PreviewTileClass) return;
	
	ClearPreview();
	ClearVisualizationConnections();

	const int TileCount = GridX * GridY;
	PreviewTiles.SetNum(TileCount);

	for (int i = 0; i < TileCount; i++)
	{
		FIntPoint TileCoord = Get2DIndex(i);
		FVector Location = FVector(TileCoord.X * Displacement,TileCoord.Y * Displacement,0);
		AGoTile* Tile = World->SpawnActor<AGoTile>(
			PreviewTileClass,
			Location,
			FRotator::ZeroRotator
		);
		Tile->AttachToActor(TileManager,FAttachmentTransformRules::KeepRelativeTransform);
		Tile->Index = i;
		PreviewTiles[i] = Tile;
	}
	BuildNeighbors();
	BuildConnections();
	VisualizeConnections();
}

void UGoBoardEditorSubsystem::LoadGridPreviewFromDataAsset(UBoardDataAsset* DataAsset)
{
	if (!DataAsset || !TileManager) return;
	GridX = DataAsset->X;
	GridY = DataAsset->Y;
	GridDisplacement = DataAsset->Displacement;
	
	UWorld* World = GetEditorWorld();
	if (!World || !PreviewTileClass) return;
	
	ClearPreview();
	ClearVisualizationConnections();
	
	const int TileCount = GridX * GridY;
	PreviewTiles.SetNum(TileCount);

	for (int i = 0; i < TileCount; i++)
	{
		const FGoTileData& TileData = DataAsset->Tiles[i];
		FIntPoint TileCoord = Get2DIndex(i);
		FVector Location = FVector(TileCoord.X * GridDisplacement,TileCoord.Y * GridDisplacement,0);
		AGoTile* Tile = World->SpawnActor<AGoTile>(
			TileData.TileClass,
			Location,
			FRotator::ZeroRotator
		);
		Tile->AttachToActor(TileManager,FAttachmentTransformRules::KeepRelativeTransform);
		
		PreviewTiles[i] = Tile;
		Tile->Connections = TileData.Connections;
		Tile->TileType = TileData.TileType;
		Tile->Walkable = TileData.Walkable;
	}
	BuildNeighbors();
	VisualizeConnections();
}

void UGoBoardEditorSubsystem::SavePreviewToDataAsset(UBoardDataAsset* DataAsset)
{
	if (!DataAsset || PreviewTiles.Num() == 0) return;

	DataAsset->Modify();
	
	DataAsset->X = GridX;
	DataAsset->Y = GridY;
	DataAsset->Displacement = GridDisplacement;
	DataAsset->Tiles.SetNum(PreviewTiles.Num());

	for (int i = 0; i < PreviewTiles.Num(); i++)
	{
		AGoTile* Tile = PreviewTiles[i];
		if (!Tile) continue;
		FGoTileData& TileData = DataAsset->Tiles[i];
		
		if (AGoTilePreview* TilePreview = Cast<AGoTilePreview>(Tile))
		{
			TileData.TileClass = TilePreview->TileClass;
		}
		else
		{
			TileData.TileClass = Tile->GetClass();
		}
		
		TileData.TileType = Tile->TileType;
		TileData.Connections = Tile->Connections;
		TileData.Walkable = Tile->Walkable;
	}

	DataAsset->MarkPackageDirty();
}

void UGoBoardEditorSubsystem::VisualizeConnections()
{
	if (!IsValid(TileManager) || !LinkMesh) return;
	UE_LOG(LogTemp, Warning, TEXT("Link mesh and tile manager are valid"));
	for (int i = 0; i < PreviewTiles.Num(); i++)
	{
		AGoTile* Tile = PreviewTiles[i];
		if (!IsValid(Tile)) continue;
		UE_LOG(LogTemp, Warning, TEXT("PreviewTile is valid"));
		
		for (int NeighborIndex : Tile->Neighbors)
		{
			if (NeighborIndex < i) continue;
			if (!AreConnected(i, NeighborIndex)) continue;
			UE_LOG(LogTemp, Warning, TEXT("Tile and neighbor are not connected"));
			AGoTile* NeighborTile = PreviewTiles[NeighborIndex];
			if (!IsValid(NeighborTile)) continue;
			UE_LOG(LogTemp, Warning, TEXT("Trying To spawn"));
			const FVector Start = Tile->GetActorLocation();
			const FVector End   = NeighborTile->GetActorLocation();
			const FVector Dir   = End - Start;
			const float Length  = Dir.Size();
			
			FTransform Transform;
			Transform.SetLocation(Start + Dir * 0.5f + FVector(0,0,75));
			Transform.SetRotation(FQuat::FindBetweenNormals(
				FVector::UpVector,
				Dir.GetSafeNormal()
			));
			Transform.SetScale3D(FVector(0.25f, 0.25f, Length / 100.f));
			UE_LOG(LogTemp, Warning, TEXT("Trying To spawn"));
			FName ComponentName = MakeUniqueObjectName(TileManager, UStaticMeshComponent::StaticClass(),TEXT("Link"));
			UStaticMeshComponent* LinkComp = NewObject<UStaticMeshComponent>(TileManager,ComponentName);
			LinkComp->SetupAttachment(TileManager->GetRootComponent());
			LinkComp->RegisterComponent();
			LinkComp->SetStaticMesh(LinkMesh);
			LinkComp->SetWorldTransform(Transform);

			Links.Add(LinkComp);
			UE_LOG(LogTemp, Warning, TEXT("Finished spawning"));
		}
	}
}
