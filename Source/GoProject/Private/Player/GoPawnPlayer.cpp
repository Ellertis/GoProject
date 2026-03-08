// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GoPawnPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/GoPlayerController.h"
#include "Core/GoGameModeBase.h"
#include "Gameplay/Enemies/GoPawnEnemySnowmen.h"
#include "Gameplay/Tile/GoTile.h"
#include "Kismet/GameplayStatics.h"

AGoPawnPlayer::AGoPawnPlayer()
{
    MeshComponent->SetCollisionResponseToChannel(ECC_Click, ECR_Block);
    HeightOffset = 100.0f;
}

void AGoPawnPlayer::BeginPlay()
{
    Super::BeginPlay();
    
    TileManager = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(this, AGoTileManager::StaticClass()));
    if(!TileManager) 
    { 
        return;
    }
    
    EnemyManager = Cast<AGoEnemyManager>(UGameplayStatics::GetActorOfClass(this, AGoEnemyManager::StaticClass()));
    if(!EnemyManager) 
    { 
        return;
    }
    
    TurnManager = Cast<AGoTurnManager>(UGameplayStatics::GetActorOfClass(this, AGoTurnManager::StaticClass()));
    if(!TurnManager) 
    {
        return;
    }
    
    TArray<AGoTile*> StartTiles = TileManager->GetTilesWithType(ETileType::Start);
    if(StartTiles.Num() == 0 || !IsValid(StartTiles[0])) 
    {
        return;
    }
    
    PlacePlayer(StartTiles[0]);
}

void AGoPawnPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    PlayerController = Cast<AGoPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    if (!PlayerController) return;
    
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}

void AGoPawnPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGoPawnPlayer::OnClickTrigger);
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Completed, this, &AGoPawnPlayer::OnClickReleased);
    	EnhancedInputComponent->BindAction(JakePlacementAction, ETriggerEvent::Triggered, this, &AGoPawnPlayer::OnJakePlacementTriggered);
    }
}

void AGoPawnPlayer::PlacePlayer(AGoTile* Tile)
{
    if (!Tile) return;
    OnMoveToTile(Tile);
}

void AGoPawnPlayer::OnClickTrigger()
{
    if (TurnManager->CurrentTurnPhase != ETurnPhase::PlayerTurn) return;
	
    if (bIsJakePlacementMode  || bIsMoving == true) return;
    
    FHitResult HitResult;
    PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Player), true, HitResult);
    
    if (this == HitResult.GetActor())
    {
        SelectedActor = this;
        ToggleHighlightNeighbors(true);
    }
}

void AGoPawnPlayer::OnClickReleased()
{
    if (TurnManager->CurrentTurnPhase != ETurnPhase::PlayerTurn) return;

	if ( bIsMoving == true) return;
	
    FHitResult HitResult;
    PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Click), true, HitResult);
    
    AGoTile* HitTile = Cast<AGoTile>(HitResult.GetActor());
    if (!IsValid(HitTile)) return;
	
    if (bIsJakePlacementMode) {TryPlaceJakeTile(HitTile);return;}
	
    if (!IsValid(CurrTile)) return;
    
    ToggleHighlightNeighbors(false);
    MoveToTile(HitTile);
}

void AGoPawnPlayer::OnJakePlacementTriggered()
{
    if (TurnManager->CurrentTurnPhase != ETurnPhase::PlayerTurn) return;

	if (bIsMoving == true) return;
	
    ToggleJakePlacementMode();
}

TArray<AGoTile*> AGoPawnPlayer::GetValidMoveTiles() const
{
    if (!TileManager || !CurrTile) return TArray<AGoTile*>();
	
    TArray<AGoTile*> ValidTiles = TileManager->GetWalkableNeighbors(CurrTile->Index);
	
    ValidTiles.RemoveAll([this](AGoTile* Tile)
    {
        if (!Tile) return true;
        return EnemyManager->IsTileOccupied(Tile->Index, nullptr);
    });
    
    return ValidTiles;
}

void AGoPawnPlayer::MoveToTile(AGoTile* Tile)
{
	if (!GetValidMoveTiles().Contains(Tile)) {UE_LOG(LogTemp, Warning, TEXT("Player move failed: Not a valid move tile"));return;}
	
	if (CurrTile == CurrentJakeTile) {UnregisterEntityOnJakeTile(this);}
	
	if (Tile->TileType == ETileType::Sandwich)
	{
		CollectSandwich();
		Tile->TileType = ETileType::Normal;
		Tile->UpdateDebugColors();
	}
	
	if (CurrTile && TileManager)
	{
		FIntPoint CurrentPos = TileManager->Get2DIndex(CurrTile->Index);
		FIntPoint TargetPos = TileManager->Get2DIndex(Tile->Index);
		FIntPoint Delta = TargetPos - CurrentPos;
		Direction = GetDirectionFromDelta(Delta);
		
		FRotator NewRotation = FRotator::ZeroRotator;
		switch (Direction)
		{
			case EFaceDirection::Xplus:  NewRotation = FRotator(0, 0, 0); break;
			case EFaceDirection::Xminus: NewRotation = FRotator(0, 180, 0); break;
			case EFaceDirection::Yplus:  NewRotation = FRotator(0, 90, 0); break;
			case EFaceDirection::Yminus: NewRotation = FRotator(0, -90, 0); break;
		}
		SetActorRotation(NewRotation);
	}
	
	StartMoveToTile(Tile, MoveDuration);
	/*
	OnMoveToTile(Tile);
	OnMoveEnd();
	FinishTurn();
	 */
}

void AGoPawnPlayer::ToggleHighlightNeighbors(bool value) const
{
    TArray<AGoTile*> Tiles = GetValidMoveTiles();
    for (AGoTile* Tile : Tiles)
    {
        Tile->HighLightTile(value);
    }
}

void AGoPawnPlayer::FinishTurn() const
{
    OnPlayerMovement.Broadcast();
}

void AGoPawnPlayer::CollectSandwich()
{
    AGoGameModeBase* GameMode = Cast<AGoGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->AddSandwich(1);
        OnCollectSandwich();
    }
}

void AGoPawnPlayer::RegisterEntityOnJakeTile(AGoPawn* Entity)
{
	if (!Entity || !CurrentJakeTile) return;
	if (Entity->CurrTile != CurrentJakeTile) return;
    
	EntitiesOnJakeTile.Add(Entity);
}

void AGoPawnPlayer::UnregisterEntityOnJakeTile(AGoPawn* Entity)
{
	if (!Entity) return;
	EntitiesOnJakeTile.Remove(Entity);
}

void AGoPawnPlayer::CheckJakeTileRemoval()
{
	if (!CurrentJakeTile) return;
	
	TSet<AGoPawn*> EntitiesToRemove;
	for (AGoPawn* Entity : EntitiesOnJakeTile)
	{
		if (!IsValid(Entity) || Entity->CurrTile != CurrentJakeTile && Entity != this) {EntitiesToRemove.Add(Entity);}
	}
    
	for (AGoPawn* Entity : EntitiesToRemove)
	{
		EntitiesOnJakeTile.Remove(Entity);
	}
	
	if (EntitiesOnJakeTile.Num() == 0) {RemoveJakeTile();}
}

void AGoPawnPlayer::OnCollectSandwich_Implementation()
{
}

void AGoPawnPlayer::ToggleJakePlacementMode()
{
    if (bIsJakePlacementMode)
    {
        ExitJakePlacementMode();
    }
    else
    {
        EnterJakePlacementMode();
    }
}

void AGoPawnPlayer::EnterJakePlacementMode()
{
    AGoGameModeBase* GameMode = Cast<AGoGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode || GameMode->GetSandwichCount() <= 0)
    {
        UE_LOG(LogTemp, Display, TEXT("Cannot enter Jake placement mode: No sandwiches available"));
        return;
    }

    if (TurnManager->CurrentTurnPhase != ETurnPhase::PlayerTurn)
    {
        UE_LOG(LogTemp, Display, TEXT("Cannot enter Jake placement mode: Not player turn"));
        return;
    }
	if (JakeIsPlaced) {return;}
	
	// If in movement mode
    if (SelectedActor == this)
    {
        ToggleHighlightNeighbors(false);
        SelectedActor = nullptr;
    }

    bIsJakePlacementMode = true;
    UpdateJakePlacementHighlights();
    OnEnterJakePlacementMode();
    
    UE_LOG(LogTemp, Display, TEXT("Entered Jake placement mode"));
}

void AGoPawnPlayer::ExitJakePlacementMode()
{
    if (bIsJakePlacementMode)
    {
        bIsJakePlacementMode = false;
        ClearJakePlacementHighlights();
        OnExitJakePlacementMode();
        
        UE_LOG(LogTemp, Display, TEXT("Exited Jake placement mode"));
    }
}

void AGoPawnPlayer::OnEnterJakePlacementMode_Implementation()
{
}

void AGoPawnPlayer::OnExitJakePlacementMode_Implementation()
{
}

void AGoPawnPlayer::UpdateJakePlacementHighlights()
{
    ClearJakePlacementHighlights();
    HighlightedJakeTiles = GetValidJakePlacementTiles();
    
    for (AGoTile* Tile : HighlightedJakeTiles)
    {
    	Tile->HighLightTile(true);
    	Tile->DebugDynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor(1,0.3,0,1)); //Debug highlight for jake tiles
    }
}

void AGoPawnPlayer::ClearJakePlacementHighlights()
{
    for (AGoTile* Tile : HighlightedJakeTiles)
    {
    	Tile->HighLightTile(false);
    	Tile->UpdateDebugColors();
    }
    HighlightedJakeTiles.Empty();
}

TArray<AGoTile*> AGoPawnPlayer::GetValidJakePlacementTiles() const
{
    TArray<AGoTile*> ValidTiles;
    
    if (!TileManager || !CurrTile) return ValidTiles;
    
    // Collect all tiles around where Jake can be placed
    TArray<AGoTile*> VoidTiles = TileManager->GetTilesWithType(ETileType::Void);
    
    FIntPoint PlayerPos = TileManager->Get2DIndex(CurrTile->Index);
    
    for (AGoTile* VoidTile : VoidTiles)
    {
        if (!IsValid(VoidTile)) continue;
        
        FIntPoint VoidPos = TileManager->Get2DIndex(VoidTile->Index);
        int Distance = FMath::Abs(PlayerPos.X - VoidPos.X) + FMath::Abs(PlayerPos.Y - VoidPos.Y);
        
        // Must be adjacent and connected
        if (Distance == 1 && TileManager->AreConnected(CurrTile->Index, VoidTile->Index))
        {
            ValidTiles.Add(VoidTile);
        }
    }
    
    return ValidTiles;
}

bool AGoPawnPlayer::TryPlaceJakeTile(AGoTile* TargetVoidTile)
{
    if (!bIsJakePlacementMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot place Jake tile: Not in placement mode"));
        return false;
    }

    AGoGameModeBase* GameMode = Cast<AGoGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        ExitJakePlacementMode();
        return false;
    }
	
    if (!TargetVoidTile || !IsValid(TargetVoidTile) || TargetVoidTile->TileType != ETileType::Void)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot place Jake tile: Invalid target tile"));
        return false;
    }
	
    TArray<AGoTile*> ValidTiles = GetValidJakePlacementTiles();
    if (!ValidTiles.Contains(TargetVoidTile)) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot place Jake tile: Target not adjacent or not connected"));
        return false;
    }
	
    if (!GameMode->UseSandwich())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot place Jake tile: Failed to consume sandwich"));
        ExitJakePlacementMode();
        return false;
    }
	
    OriginalVoidTile = TargetVoidTile;
	
    FIntPoint GridPos = TileManager->Get2DIndex(TargetVoidTile->Index);
    FVector TileManagerLocation = TileManager->GetActorLocation();
	
    FVector SpawnLocation = TileManagerLocation + FVector(
        GridPos.X * TileManager->Displacement,
        GridPos.Y * TileManager->Displacement,
        0.0f
    );
	
    CurrentJakeTile = GetWorld()->SpawnActor<AGoTile>(
        JakeTileClass,
        SpawnLocation,
        FRotator::ZeroRotator
    );
    
    if (CurrentJakeTile)
    {
        CurrentJakeTile->TileType = ETileType::Jake;
        CurrentJakeTile->Walkable = true;
        CurrentJakeTile->Connections = TargetVoidTile->Connections;
        CurrentJakeTile->Neighbors = TargetVoidTile->Neighbors;
        CurrentJakeTile->Index = TargetVoidTile->Index;
        CurrentJakeTile->UpdateDebugColors();
    	
        TargetVoidTile->SetActorHiddenInGame(true);
        TargetVoidTile->SetActorEnableCollision(false);
    	
        TileManager->Tiles[TargetVoidTile->Index] = CurrentJakeTile;
        TileManager->VisualizeConnections();

    	EntitiesOnJakeTile.Add(this);
        
        OnPlaceJakeTile();
        ExitJakePlacementMode();
    	JakeIsPlaced = true;
        FinishTurn();
        
        return true;
    }
	
    UE_LOG(LogTemp, Error, TEXT("Failed to spawn Jake tile - refunding sandwich"));
    GameMode->AddSandwich(1);
    ExitJakePlacementMode();
    return false;
}

void AGoPawnPlayer::OnPlaceJakeTile_Implementation()
{
}

void AGoPawnPlayer::RemoveJakeTile()
{
	if (!CurrentJakeTile || !OriginalVoidTile) return;
	JakeIsPlaced = false;
	
	OriginalVoidTile->SetActorHiddenInGame(false);
	OriginalVoidTile->SetActorEnableCollision(true);
	int Index = CurrentJakeTile->Index;
	TileManager->Tiles[Index] = OriginalVoidTile;
	CurrentJakeTile->Destroy();
	CurrentJakeTile = nullptr;
	EntitiesOnJakeTile.Empty();
	TileManager->VisualizeConnections();
	
	OnRemoveJakeTile();
	UE_LOG(LogTemp, Display, TEXT("Jake tile removed"));
}

void AGoPawnPlayer::OnRemoveJakeTile_Implementation()
{
}