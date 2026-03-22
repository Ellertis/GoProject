// Fill out your copyright notice in the Description page of Project Settings.

#include "Gameplay/Enemies/GoSnowball.h"
#include "Gameplay/Enemies/GoPawnEnemyGunter.h"
#include "Player/GoPawnPlayer.h"
#include "Core/GoGameModeBase.h"

AGoSnowball::AGoSnowball()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetSphereRadius(50.0f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGoSnowball::OnOverlap);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGoSnowball::Launch(const FVector& Start, const FVector& End)
{
    SetActorLocation(Start);
    TargetLocation = End;
    bMoving = true;
    
    if (!EnemyManager) return;
    EnemyManager->RegisterSnowball(this);
}

void AGoSnowball::BeginPlay()
{
    Super::BeginPlay();
}

void AGoSnowball::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AGoSnowball::StaticClass())) return;
    
    OnSnowballHit(OtherActor);
    
    if (AGoPawnPlayer* Player = Cast<AGoPawnPlayer>(OtherActor))
    {
        Player->OnDamageTaken();
    }
    else if (AGoPawnEnemyGunter* Gunter = Cast<AGoPawnEnemyGunter>(OtherActor))
    {
        if(Gunter->bWasHitThisTurn == false) {Gunter->ApplyDamage(1, ProjectedDirection);}
    }
    
    if (EnemyManager)
    {
        EnemyManager->UnregisterSnowball(this);
    }
    
    OnSnowballDestroyed();
    Destroy();
}

void AGoSnowball::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!bMoving) return;

    FVector Current = GetActorLocation();
    FVector Direction = (TargetLocation - Current).GetSafeNormal();
    float Distance = FVector::Dist(Current, TargetLocation);
    float Step = Speed * DeltaTime;

    if (Distance <= Step || Current.Equals(TargetLocation, 10.0f))
    {
        SetActorLocation(TargetLocation);
        bMoving = false;
        
        if (EnemyManager)
        {
            EnemyManager->UnregisterSnowball(this);
        }
        
        OnSnowballDestroyed();
        Destroy();
    }
    else
    {
        SetActorLocation(Current + Direction * Step);
    }
}