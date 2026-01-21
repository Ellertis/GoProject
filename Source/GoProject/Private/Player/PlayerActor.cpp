// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerActor.h"

// Sets default values
APlayerActor::APlayerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	MeshComponent->SetStaticMesh(PlayerMesh);

}

// Called when the game starts or when spawned
void APlayerActor::BeginPlay()
{
	Super::BeginPlay();

	TM = ATileManager::Get(GetWorld());
	
	if (IsValid(TM->Tiles[0]))
	{
		CurrTile = TM->Tiles[0];
	}
	
}

void APlayerActor::MoveToTile(ATile* Tile)
{
	if (!GetValidMoveTiles().Contains(Tile)) return;
	CurrTile = Tile;
	SetActorLocation(Tile->GetActorLocation()+FVector(0,0,GetActorLocation().Z));
}

// Called every frame
void APlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<ATile*> APlayerActor::GetValidMoveTiles() const
{
	return TM->GetWalkableNeighbors(CurrTile);
}

