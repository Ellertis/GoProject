// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Gameplay/Tile/GoTile.h"
#include "GoPawn.generated.h"

UCLASS()
class GOPROJECT_API AGoPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoPawn();

	//Components
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	//Variables

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Pawn|Tile")
	AGoTile* CurrTile;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
