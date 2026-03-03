// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GoPlayerController.generated.h"

UCLASS()
class GOPROJECT_API AGoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGoPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};