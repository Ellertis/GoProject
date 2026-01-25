// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GoGameModeBase.h"

#include "GameFramework/PlayerStart.h"
#include "Gameplay/Tile/GoTileManager.h"
#include "Kismet/GameplayStatics.h"

void AGoGameModeBase::BeginPlay()
{
    Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerController)) return;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(
		Handle,
		this,
		&AGoGameModeBase::OnGridReady,
		0.25f,
		false
	);
	
	/* TODO Set proper Start/End tile then, spawn on start

	*/ 
}

void AGoGameModeBase::SpawnPlayer()
{
	/* BROKEN!!! NOT FINISHED
	AGoTileManager* TileManager =
		Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(this, AGoTileManager::StaticClass()));
	*/
	if (!PlayerPawnClass) return;
	/*
	for (AGoTile* Tile : TileManager->Tiles)
	{
		if (Tile->TileType != ETileType::Start) continue;
		StartTile = Tile;
		break;
	}
	
	AGoTile* StartTile = TileManager->Tiles[0];
	if (!StartTile) return;
	*/
	//FVector Location = StartTile->GetActorLocation();
	FVector Location = FVector(-170, -1380.0, 0);
	Location.Z += 100.0f;

	PlayerPawn = GetWorld()->SpawnActor<AGoPlayerPawn>(
		PlayerPawnClass,
		Location,
		FRotator::ZeroRotator
	);
	
}

void AGoGameModeBase::SpawnCamera()
{
	if (!CameraActorClass)return;

	APlayerStart* PlayerStart = Cast<APlayerStart>(
		UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass()));
	if (!PlayerStart)return;

	FVector CameraLocation = PlayerStart->GetActorLocation();
	FRotator CameraRotation = PlayerStart->GetActorRotation();

	CameraActor = GetWorld()->SpawnActor<AGoCameraActor>(
		CameraActorClass,
		CameraLocation,
		CameraRotation
	);
}

void AGoGameModeBase::OnGridReady()
{
	SpawnPlayer();
	SpawnCamera();

	if(IsValid(PlayerPawn)) PlayerController->Possess(PlayerPawn);
	if(IsValid(CameraActor)) PlayerController->SetViewTarget(CameraActor);
}
