// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GoGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AGoGameModeBase::BeginPlay()
{
    Super::BeginPlay();
	
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
}

void AGoGameModeBase::SpawnCamera()
{
	if (!CameraActorClass) return;

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

	PlayerController->SetViewTarget(CameraActor);
}