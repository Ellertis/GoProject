// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/StaticMesh.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "PlayerActor.h"
#include "GoCameraPawn.generated.h"

UCLASS()
class GOPROJECT_API AGoCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called on possession
	virtual void PossessedBy(AController* NewController) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetPlayerActor(APlayerActor* PlayerRef) { PlayerActor = PlayerRef; }

protected:
	//Components
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* Root;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* ClickAction;

	UPROPERTY()
	APlayerActor* PlayerActor;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AActor* SelectedActor;
	
private:
	void OnClickTrigger();
	void OnClickReleased();
	
};
