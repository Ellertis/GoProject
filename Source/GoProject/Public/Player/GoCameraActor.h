// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoCameraActor.generated.h"

class UCameraComponent;

UCLASS(Blueprintable)
class GOPROJECT_API AGoCameraActor : public AActor
{
	GENERATED_BODY()
    
public:    
	AGoCameraActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
	void OnCameraFocus(AGoTile* FocusTile);
};