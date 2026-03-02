// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GoGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"

void AGoGameModeBase::BeginPlay()
{
    Super::BeginPlay();
	TM = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGoTileManager::StaticClass()));
	if(!TM) return;
	SpawnEnemyManager();
	if(!EnemyManager)return;
	SpawnTurnManager();
	if(!TurnManager)return;
	
	TurnManager->OnTurnPhaseChanged.AddDynamic(EnemyManager,&AGoEnemyManager::OnNewEnemyTurn);
	
	EnemyManager->NoRemainingEnemyTurns.AddDynamic(TurnManager,&AGoTurnManager::OnNoEnemyTurnsLeft);
	EnemyManager->GunterIsDead.AddDynamic(this, &AGoGameModeBase::AGoGameModeBase::GameOver);
	EnemyManager->TileManager = TM;
	
	TM->OnGridGenerated.AddDynamic(this, &AGoGameModeBase::OnGridGenerated);
	TM->EnemyManager = EnemyManager;
	TM->LoadGridFromDataAsset(TM->DataAsset);
}

void AGoGameModeBase::GameOver()
{
	// Call UI //Restart Level 
}

AGoPawnPlayer* AGoGameModeBase::GetPlayer() const
{
	return PlayerPawn;
}

void AGoGameModeBase::OnGridGenerated()
{
	SpawnPlayer();
	PlayerPawn->TurnManager = TurnManager;
	PlayerPawn->OnPlayerMovement.AddDynamic(TurnManager,&AGoTurnManager::PlayerMoved);
	SpawnCamera();
	EnemyManager->PlayerRef = GetPlayer();
	TurnManager->StartGame();
}

void AGoGameModeBase::SpawnPlayer()
{
	if(!PlayerPawnClass) return;
	
	PlayerPawn = GetWorld()->SpawnActor<AGoPawnPlayer>(
		PlayerPawnClass,
		FVector(0, 0, 0),
		FRotator::ZeroRotator
	);
	
	PlayerController = Cast<AGoPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(!PlayerController) return;
	PlayerController->Possess(PlayerPawn);
	UE_LOG(LogTemp, Display, TEXT("SpawnPlayer & Possess"));
}

void AGoGameModeBase::SpawnCamera()
{
	if (!CameraActorClass) return;

	APlayerStart* PlayerStart = Cast<APlayerStart>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
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

void AGoGameModeBase::SpawnEnemyManager()
{
	if(!EnemyManagerClass)return;
	EnemyManager = GetWorld()->SpawnActor<AGoEnemyManager>(
		EnemyManagerClass,
		FVector(0, 0, 0),
		FRotator::ZeroRotator
	);
}

void AGoGameModeBase::SpawnTurnManager()
{
	if(!TurnManagerClass)return;
	TurnManager = GetWorld()->SpawnActor<AGoTurnManager>(
		TurnManagerClass,
		FVector(0, 0, 0),
		FRotator::ZeroRotator
	);
}