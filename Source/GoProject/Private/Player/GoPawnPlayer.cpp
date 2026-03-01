// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoPawnPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/GoPlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGoPawnPlayer::AGoPawnPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComponent->SetCollisionResponseToChannel(ECC_Click,ECR_Block);
}

// Called when the game starts or when spawned
void AGoPawnPlayer::BeginPlay()
{
	Super::BeginPlay();
	TM =  Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(this, AGoTileManager::StaticClass()));
	if(!TM) {UE_LOG(LogTemp, Warning, TEXT("GoPlayerPawn : Tile Manager not found")); return;}
	
	TArray<AGoTile*> StarTiles = TM->GetTilesWithType(ETileType::Start);
	if(StarTiles.Num() == 0 || !IsValid(StarTiles[0])) {UE_LOG(LogTemp, Warning, TEXT("GoPlayerPawn : Start Tile not Found")); return;}
	
	PlacePlayer(StarTiles[0]);
}

// Called every frame
void AGoPawnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
	
void AGoPawnPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<AGoPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PlayerController) return;
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem=
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext,0);
		}
}

// Called to bind functionality to input
void AGoPawnPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGoPawnPlayer::OnClickTrigger);
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Completed, this,  &AGoPawnPlayer::OnClickReleased);
		//EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AGoCameraPawn::OnClickTrigger);
	}
}

void AGoPawnPlayer::PlacePlayer(AGoTile* Tile)
{
	CurrTile = Tile;
	FVector StartLocation = CurrTile->GetActorLocation();
	StartLocation = StartLocation + FVector(0, 0, 100);
	SetActorLocation(StartLocation);
	
}

void AGoPawnPlayer::OnClickTrigger()
{
	if(TurnManager->CurrentTurnPhase != ETurnPhase::PlayerTurn) return;
	UE_LOG(LogTemp, Warning, TEXT("OnClickTrigger"));
	//if(!bCanClickTile) return; TODO
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Click), true, HitResult);
	if (this == Cast<AGoPawnPlayer>(HitResult.GetActor()))
	{
		SelectedActor = this;
		ToggleHighlightNeighbors(true);
	}
	UE_LOG(LogTemp, Warning, TEXT("ReadyToMove"));
}

void AGoPawnPlayer::OnClickReleased()
{
	if(TurnManager->CurrentTurnPhase != ETurnPhase::PlayerTurn) return;
	UE_LOG(LogTemp, Warning, TEXT("OnClickReleased"));
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Click), true,HitResult);
	AGoTile* HitTile = Cast<AGoTile>(HitResult.GetActor());
	if (!IsValid(HitTile)) return;
	UE_LOG(LogTemp, Warning, TEXT("Clicked Tile"));
	
	if (!IsValid(CurrTile)) return;
	ToggleHighlightNeighbors(false);
	MoveToTile(HitTile);
}

TArray<AGoTile*> AGoPawnPlayer::GetValidMoveTiles() const
{
	return TM->GetWalkableNeighbors(CurrTile->Index);
}

void AGoPawnPlayer::MoveToTile(AGoTile* Tile)
{
	if (!GetValidMoveTiles().Contains(Tile)) return;
	SetActorLocation(Tile->GetActorLocation()+FVector(0,0,100));
	CurrTile = Tile;

	//await player arrive to tile
	FinishTurn();
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
	UE_LOG(LogTemp,Warning,TEXT("Called OnPlayerMovement.Broadcast"));
}
