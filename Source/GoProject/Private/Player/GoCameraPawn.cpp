// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoCameraPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Gameplay/Tile/Tile.h"

// Sets default values
AGoCameraPawn::AGoCameraPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AGoCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());
}

// Called every frame
void AGoCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called on possession
void AGoCameraPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (APlayerController* TempPlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem=
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(TempPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext,0);
		}
	}
}

// Called to bind functionality to input
void AGoCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGoCameraPawn::OnClickTrigger);
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Completed, this,  &AGoCameraPawn::OnClickReleased);
	}
}


void AGoCameraPawn::OnClickTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickTrigger"));
	APlayerController* TempPlayerController = Cast<APlayerController>(GetController()); //sus
	FVector2D MousePos;
	FVector WorldOrigin, WorldDir;
	TempPlayerController->GetMousePosition(MousePos.X, MousePos.Y);
	TempPlayerController->DeprojectScreenPositionToWorld(MousePos.X, MousePos.Y, WorldOrigin, WorldDir);
	FVector End = WorldOrigin + WorldDir * 10000.f;
	FHitResult Hit;
	APlayerActor* HitPlayerActor;
	if (GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, End, ECC_Visibility))
	{
		if (APlayerActor* Player = Cast<APlayerActor>(Hit.GetActor()))
		{
			HitPlayerActor = Player; //The cast is useless First click on the actor. Highlight available tiles, then click on the highlighted tile
			UE_LOG(LogTemp, Warning, TEXT("ReadyToMove"));
		}
	}
	
}

void AGoCameraPawn::OnClickReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickReleased"));
	APlayerController* TempPlayerController = Cast<APlayerController>(GetController()); //sus
	FVector2D MousePos;
	FVector WorldOrigin, WorldDir;
	TempPlayerController->GetMousePosition(MousePos.X, MousePos.Y);
	TempPlayerController->DeprojectScreenPositionToWorld(MousePos.X, MousePos.Y, WorldOrigin, WorldDir);
	FVector End = WorldOrigin + WorldDir * 10000.f;
	FHitResult Hit;
	ATile* HitTile = nullptr;
	if (GetWorld()->LineTraceSingleByChannel(Hit, WorldOrigin, End, ECC_Visibility))
	{
		if (ATile* Tile = Cast<ATile>(Hit.GetActor()))
		{
			HitTile = Tile;
			UE_LOG(LogTemp, Warning, TEXT("Clicked Tile"));
		}
	}
	
	if (!PlayerActor || !PlayerActor->CurrTile) return;
	if (!HitTile)return;
	PlayerActor->MoveToTile(HitTile);
	UE_LOG(LogTemp, Warning, TEXT("OnClickTrigger Neighbor"));
	
}

