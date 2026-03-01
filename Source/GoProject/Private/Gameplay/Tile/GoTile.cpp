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
	MeshComponent->SetCollisionResponseToChannel(ECC_Click,ECR_Block);
	
	DebugMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugStaticMesh"));
	DebugMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	DebugMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DebugMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	DebugMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	
}

void AGoTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsValid(Mesh)){MeshComponent->SetStaticMesh(Mesh);DebugMeshComponent->SetStaticMesh(Mesh);}
	
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	MeshComponent->SetMaterial(0, DynamicMaterial);
	
	DebugDynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	DebugMeshComponent->SetMaterial(0, DebugDynamicMaterial);

	UpdateDebugColors();
}

// Called when the game starts or when spawned
void AGoTile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGoTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoTile::UpdateDebugColors() const
{
	switch(TileType)
	{
		case ETileType::Start: DebugDynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Green);break;
		case ETileType::End: DebugDynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);break;
		default: DebugDynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Black);break;
	}
}

void AGoTile::HighLightTile(bool value)
{
	value ? DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Green) : DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::White);
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

