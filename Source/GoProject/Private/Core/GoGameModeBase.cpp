// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GoGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AGoGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    
    TM = Cast<AGoTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGoTileManager::StaticClass()));
    if(!TM) return;
    
    SpawnEnemyManager();
    if(!EnemyManager) return;
    
    SpawnTurnManager();
    if(!TurnManager) return;
    
    TurnManager->OnTurnPhaseChanged.AddDynamic(EnemyManager, &AGoEnemyManager::OnNewEnemyTurn);
    
    EnemyManager->NoRemainingEnemyTurns.AddDynamic(TurnManager, &AGoTurnManager::OnNoEnemyTurnsLeft);
    EnemyManager->GunterIsDead.AddDynamic(this, &AGoGameModeBase::GameOver);
    EnemyManager->GunterEnd.AddDynamic(this, &AGoGameModeBase::GameWin);
    EnemyManager->TileManager = TM;
    
    TM->OnGridGenerated.AddDynamic(this, &AGoGameModeBase::OnGridGenerated);
    TM->EnemyManager = EnemyManager;
    TM->LoadGridFromDataAsset(TM->DataAsset);
}

void AGoGameModeBase::GameOver()
{
    OnGameOver.Broadcast();
    // UI will be handled by Blueprint
}

void AGoGameModeBase::GameWin()
{
    OnGameWin.Broadcast();
}

void AGoGameModeBase::GameStart()
{
    OnGameStart.Broadcast();
}

void AGoGameModeBase::RestartLevel()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void AGoGameModeBase::OnGridGenerated()
{   
    SpawnPlayer();
    PlayerPawn->TurnManager = TurnManager;
    PlayerPawn->OnPlayerMovement.AddDynamic(TurnManager, &AGoTurnManager::PlayerMoved);
    EnemyManager->PlayerRef = PlayerPawn;
    
    SpawnCamera();
    TurnManager->StartGame();
    OnGameStart.Broadcast();
}

void AGoGameModeBase::SpawnPlayer()
{
    if(!PlayerPawnClass) {UE_LOG(LogTemp, Error, TEXT("GoGameModeBase: PlayerPawnClass is null"));return;}
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    PlayerPawn = GetWorld()->SpawnActor<AGoPawnPlayer>(
        PlayerPawnClass,
        FVector(0, 0, 0),
        FRotator::ZeroRotator,
        SpawnParams
    );
    
    if (!PlayerPawn) {UE_LOG(LogTemp, Error, TEXT("GoGameModeBase: Failed to spawn player"));return;}
    
    PlayerController = Cast<AGoPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    PlayerController->Possess(PlayerPawn);
}

void AGoGameModeBase::SpawnCamera()
{
    if (!CameraActorClass) {UE_LOG(LogTemp, Error, TEXT("GoGameModeBase: CameraActorClass is null"));return;}

    APlayerStart* PlayerStart = Cast<APlayerStart>(
        UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));
    
    FVector CameraLocation = FVector::ZeroVector;
    FRotator CameraRotation = FRotator::ZeroRotator;
    
    if (PlayerStart)
    {
        CameraLocation = PlayerStart->GetActorLocation();
        CameraRotation = PlayerStart->GetActorRotation();
    }
    
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    CameraActor = GetWorld()->SpawnActor<AGoCameraActor>(
        CameraActorClass,
        CameraLocation,
        CameraRotation,
        SpawnParams
    );

    if (!CameraActor) {UE_LOG(LogTemp, Error, TEXT("GoGameModeBase: Failed to spawn camera"));return;}
    PlayerController->SetViewTarget(CameraActor);
}

void AGoGameModeBase::SpawnEnemyManager()
{
    if(!EnemyManagerClass) return;
    EnemyManager = GetWorld()->SpawnActor<AGoEnemyManager>(
        EnemyManagerClass,
        FVector(0, 0, 0),
        FRotator::ZeroRotator
    );
}

void AGoGameModeBase::SpawnTurnManager()
{
    if(!TurnManagerClass) return;
    TurnManager = GetWorld()->SpawnActor<AGoTurnManager>(
        TurnManagerClass,
        FVector(0, 0, 0),
        FRotator::ZeroRotator
    );
}