// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GoGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AGoGameModeBase::BeginPlay()
{
    Super::BeginPlay();
	TM = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(this, AGoTileManager::StaticClass()));
	if(!TM) return;
	TM->OnGridGenerated.AddDynamic(this, &AGoGameModeBase::OnGridGenerated);
	TM->LoadGridFromDataAsset(TM->DataAsset);

}

void AGoGameModeBase::OnGridGenerated()
{
	SpawnPlayer();
	SpawnCamera();
}

void AGoGameModeBase::SpawnPlayer()
{
	if(!PlayerPawnClass) return;
	
	PlayerPawn = GetWorld()->SpawnActor<AGoPawnPlayer>(
		PlayerPawnClass,
		FVector(0, 0, 0),
		FRotator::ZeroRotator
	);
	
	PlayerController = Cast<AGoPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(!PlayerController) return;
	PlayerController->Possess(PlayerPawn);
	UE_LOG(LogTemp, Display, TEXT("SpawnPlayer & Possess"));
}

void AGoGameModeBase::SpawnCamera()
{
	if (!CameraActorClass) return;

	APlayerStart* PlayerStart = Cast<APlayerStart>(
		UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass()));
	if (!PlayerStart) return;

	FVector CameraLocation = PlayerStart->GetActorLocation();
	FRotator CameraRotation = PlayerStart->GetActorRotation();

	CameraActor = GetWorld()->SpawnActor<AGoCameraActor>(
		CameraActorClass,
		CameraLocation,
		CameraRotation
	);

	PlayerController->SetViewTarget(CameraActor);
	UE_LOG(LogTemp, Display, TEXT("Spawn Camera & Finished"));
}