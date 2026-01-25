// Fill out your copyright notice in the Description page of Project Settings.



#include "Player/GoPlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

#include "Gameplay/Tile/GoTile.h"

// Sets default values
AGoPlayerPawn::AGoPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AGoPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	
}

// Called every frame
void AGoPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem=
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext,0);
		}
}

// Called to bind functionality to input
void AGoPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AGoPlayerPawn::OnClickTrigger);
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Completed, this,  &AGoPlayerPawn::OnClickReleased);
		//EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AGoCameraPawn::OnClickTrigger);
	}
}

void AGoPlayerPawn::OnClickTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickTrigger"));
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	if (this == Cast<AGoPlayerPawn>(HitResult.GetActor()))
	{
		SelectedActor = this;
		ToggleHighlightNeighbors(true);
	}
	//Player; //The cast is useless First click on the actor. Highlight available tiles, then click on the highlighted tile
	UE_LOG(LogTemp, Warning, TEXT("ReadyToMove"));
	
	
}

void AGoPlayerPawn::OnClickReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("OnClickReleased"));
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true,HitResult);
	AGoTile* HitTile = Cast<AGoTile>(HitResult.GetActor());
	if (!IsValid(HitTile)) return;
	UE_LOG(LogTemp, Warning, TEXT("Clicked Tile"));
	
	if (IsValid(CurrTile)) return;
	ToggleHighlightNeighbors(false);
	MoveToTile(HitTile);
}

TArray<AGoTile*> AGoPlayerPawn::GetValidMoveTiles() const
{
	return TM->GetWalkableNeighbors(CurrTile->Index);
}

void AGoPlayerPawn::MoveToTile(AGoTile* Tile)
{
	if (!GetValidMoveTiles().Contains(Tile)) return;
	SetActorLocation(Tile->GetActorLocation()+FVector(0,0,100));
	CurrTile = Tile;
}

void AGoPlayerPawn::ToggleHighlightNeighbors(bool value) const
{
	TArray<AGoTile*> Tiles = GetValidMoveTiles();
	for (AGoTile* Tile : Tiles)
	{
		Tile->HighLightTile(value);
	}
}


