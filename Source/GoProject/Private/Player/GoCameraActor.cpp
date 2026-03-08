// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/GoCameraActor.h"
#include "Camera/CameraComponent.h"

AGoCameraActor::AGoCameraActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = SceneComponent;
    /* old camera | legacy code
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(RootComponent);
    */
    CineCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("CineCameraComponent"));
    CineCameraComponent->SetupAttachment(RootComponent);
}