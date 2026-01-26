// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Tile/GoTile.h"

// Sets default values
AGoTile::AGoTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	
}

void AGoTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IsValid(Mesh)){MeshComponent->SetStaticMesh(Mesh);}
}

// Called when the game starts or when spawned
void AGoTile::BeginPlay()
{
	Super::BeginPlay();
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	MeshComponent->SetMaterial(0, DynamicMaterial);
}

// Called every frame
void AGoTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoTile::HighLightTile(bool value)
{
	if (value) DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Green);
	if (!value) DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::White);
}

bool AGoTile::HasConnections(ETileConnection Dir) const
{
	return (Connections & static_cast<int32>(Dir)) != 0;
}

void AGoTile::AddConnections(ETileConnection Dir)
{
	Connections |= static_cast<int32>(Dir);
}

void AGoTile::RemoveConnections(ETileConnection Dir)
{
	Connections &= ~static_cast<int32>(Dir);
}

void AGoTile::ClearConnections()
{
	Connections = 0;
}

