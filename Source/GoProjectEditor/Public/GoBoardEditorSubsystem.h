// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "GoTilePreview.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "Gameplay/Tile/GoTile.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "GoBoardEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GOPROJECTEDITOR_API UGoBoardEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GoBoard|Editor")
	TSubclassOf<AGoTilePreview> PreviewTileClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GoBoard|Editor")
	AGoTileManager* TileManager;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GoBoard|Editor")
	UStaticMesh* LinkMesh;
	
	UFUNCTION(BlueprintCallable, Category="GoBoard|Editor")
	void GenerateGrid(int X,int Y,int Displacement);

	UFUNCTION(BlueprintCallable, Category="GoBoard|Editor")
	void LoadGridPreviewFromDataAsset(UBoardDataAsset* DataAsset);
	
	UFUNCTION(BlueprintCallable, Category="GoBoard|Editor")
	void SavePreviewToDataAsset(UBoardDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable, Category="GoBoard|Editor")
	void VisualizeConnections();

	UFUNCTION(BlueprintCallable, Category="GoBoard|Editor")
	void ClearPreview();

	UFUNCTION(BlueprintCallable, Category="GoBoard|Editor")
	void ClearVisualizationConnections();
	
	void BuildNeighbors();
	void BuildConnections();
	void ClearVoidTiles();
	bool AreConnected(int TileIndA, int TileIndB) const;
	FIntPoint GetDeltaIndex(int TileAInd, int TileBInd) const;

	
	

private:
	UPROPERTY(Transient)
	TArray<AGoTile*> PreviewTiles;

	UPROPERTY(Transient)
	TArray<UStaticMeshComponent*> Links;
	 
	UWorld* GetEditorWorld() const;

	UPROPERTY(Transient)
	int GridX;

	UPROPERTY(Transient)
	int GridY;

	UPROPERTY(Transient)
	int GridDisplacement;
	
	FIntPoint Get2DIndex(int TileInd) const;
	int Get1DIndex(int TileX, int TileY) const;
	
};
