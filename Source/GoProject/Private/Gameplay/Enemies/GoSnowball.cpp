// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Enemies/GoSnowball.h"

#include "Gameplay/Enemies/GoPawnEnemyGunter.h"
#include "Player/GoPawnPlayer.h"

// Sets default values
AGoSnowball::AGoSnowball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetSphereRadius(50.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGoSnowball::OnOverlap); // use overlap for simplicity

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGoSnowball::Launch(const FVector& Start, const FVector& End)
{
	SetActorLocation(Start);
	TargetLocation = End;
	bMoving = true;
}

// Called when the game starts or when spawned
void AGoSnowball::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGoSnowball::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;
	if (AGoPawnPlayer* Player = Cast<AGoPawnPlayer>(OtherActor))
	{
		//Death Screen game over
		Destroy();
	}
	else if (AGoPawnEnemyGunter* Gunter = Cast<AGoPawnEnemyGunter>(OtherActor))
	{
		Gunter->ApplyDamage(1);
	}
	else if (AGoSnowball* Snowball = Cast<AGoSnowball>(OtherActor))
	{
		Destroy();
	}
}

// Called every frame
void AGoSnowball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bMoving) return;

	FVector Current = GetActorLocation();
	FVector Direction = (TargetLocation - Current).GetSafeNormal();
	float Distance = FVector::Dist(Current, TargetLocation);
	float Step = Speed * DeltaTime;

	if (GetActorLocation().Equals(TargetLocation))
	{
		SetActorLocation(TargetLocation);
		bMoving = false;
		Destroy(); // reached target without hitting anything
	}
	else
	{
		SetActorLocation(Current + Direction * Step);
	}
}

