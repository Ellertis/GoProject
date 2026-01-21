// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GoGameModeBase.h"

#include "GameFramework/PlayerStart.h"
#include "Gameplay/Tile/TileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerActor.h"

void AGoGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    SpawnPlayer();
    SpawnCameraPawn();

	if (CameraPawn && PlayerActor)
	{
		CameraPawn->SetPlayerActor(PlayerActor);
	}
}

void AGoGameModeBase::SpawnPlayer()
{
	ATileManager* TileManager =
		Cast<ATileManager>(UGameplayStatics::GetActorOfClass(this, ATileManager::StaticClass()));
	if (!TileManager || !TileManager->Tiles.IsValidIndex(0) ||!PlayerActorClass)return;

	ATile* StartTile = TileManager->Tiles[0];
	if (!StartTile)return;

	FVector Location = StartTile->GetActorLocation();
	Location.Z += 100.0f;

	PlayerActor = GetWorld()->SpawnActor<APlayerActor>(
		PlayerActorClass,
		Location,
		FRotator::ZeroRotator
	);
}

void AGoGameModeBase::SpawnCameraPawn()
{
	if (!CameraPawnClass)return;

	APlayerStart* PlayerStart = Cast<APlayerStart>(
		UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass()));
	if (!PlayerStart)return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)return;

	FVector CameraLocation = PlayerStart->GetActorLocation();
	FRotator CameraRotation = PlayerStart->GetActorRotation();

	CameraPawn = GetWorld()->SpawnActor<AGoCameraPawn>(
		CameraPawnClass,
		CameraLocation,
		CameraRotation
	);

	if (CameraPawn)
	{
		PlayerController->Possess(CameraPawn);
	}
}
