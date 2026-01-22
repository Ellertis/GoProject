// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoCameraPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Gameplay/Tile/Tile.h"
#include "Kismet/GameplayStatics.h"

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
	PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
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
		//EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AGoCameraPawn::OnClickTrigger);
	}
}

void AGoCameraPawn::OnClickTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickTrigger"));
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	if (APlayerActor* Player = Cast<APlayerActor>(HitResult.GetActor()))
	{
		if (!IsValid(Player)) return;
		SelectedActor = Player;
		PlayerActor = Player;
		PlayerActor->ToggleHighlightNeighbors(true);
	}
	//Player; //The cast is useless First click on the actor. Highlight available tiles, then click on the highlighted tile
	UE_LOG(LogTemp, Warning, TEXT("ReadyToMove"));
	
	
}

void AGoCameraPawn::OnClickReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickReleased"));
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true,HitResult);
	ATile* HitTile = Cast<ATile>(HitResult.GetActor());
	if (!IsValid(HitTile)) return;
	UE_LOG(LogTemp, Warning, TEXT("Clicked Tile"));
	
	if (!PlayerActor || !PlayerActor->CurrTile) return;
	PlayerActor->ToggleHighlightNeighbors(false);
	PlayerActor->MoveToTile(HitTile);
	
}

