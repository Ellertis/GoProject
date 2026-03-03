// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GoTurnManager.generated.h"

UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	None UMETA(DisplayName = "None"),
	PlayerTurn UMETA(DisplayName = "Player Turn"),
	EnemyTurn UMETA(DisplayName = "Enemy Turn"),
	Resolve UMETA(DisplayName = "Resolve")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnPhaseChanged, ETurnPhase, NewTurnPhase);

UCLASS(Blueprintable)
class GOPROJECT_API AGoTurnManager : public AActor
{
	GENERATED_BODY()
    
public:    
	AGoTurnManager();

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION()
	void PlayerMoved();

	UFUNCTION()
	void OnNoEnemyTurnsLeft();
    
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	ETurnPhase CurrentTurnPhase = ETurnPhase::None;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnTurnPhaseChanged OnTurnPhaseChanged;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Turn")
	void OnPlayerTurnStarted();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Turn")
	void OnEnemyTurnStarted();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Turn")
	void OnTurnEnded();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetTurnPhase(ETurnPhase NewTurnPhase);
};