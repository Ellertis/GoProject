// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoCameraActor.h"

// Sets default values
AGoCameraActor::AGoCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Root = RootComponent;

}

// Called when the game starts or when spawned
void AGoCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

