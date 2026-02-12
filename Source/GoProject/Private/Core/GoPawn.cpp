// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GoPawn.h"

// Sets default values
AGoPawn::AGoPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AGoPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGoPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

