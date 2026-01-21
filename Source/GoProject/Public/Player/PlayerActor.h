// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Tile/TileManager.h"
#include "PlayerActor.generated.h"

UCLASS()
class GOPROJECT_API APlayerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerActor();

	//Components
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* PlayerMesh;
	
	UPROPERTY()
	ATileManager* TM;
	
	UPROPERTY()
	ATile* CurrTile;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<ATile*> GetValidMoveTiles() const;

	void MoveToTile(ATile* Tile);
	
};
