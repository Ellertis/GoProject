// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/GoPlayerController.h"

AGoPlayerController::AGoPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AGoPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AGoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}