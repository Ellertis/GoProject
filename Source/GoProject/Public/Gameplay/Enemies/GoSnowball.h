// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "GoSnowball.generated.h"

class AGoEnemyManager;

UCLASS(Blueprintable)
class GOPROJECT_API AGoSnowball : public AActor
{
	GENERATED_BODY()
    
public:    
	AGoSnowball();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* CollisionSphere;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite)
	AGoEnemyManager* EnemyManager;
    
	UPROPERTY(BlueprintReadWrite)
	EFaceDirection ProjectedDirection;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 1000.0f;

	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation;
    
	UPROPERTY(BlueprintReadOnly)
	bool bMoving = false;

	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& Start, const FVector& End);
    
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
				   UPrimitiveComponent* OtherComp, int OtherBodyIndex, 
				   bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void OnSnowballHit(AActor* HitActor);
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Effects")
	void OnSnowballDestroyed();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};