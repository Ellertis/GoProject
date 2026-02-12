// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Gameplay/Tile/BoardDataAsset.h"
#include "GoEditorUtilityWidget.generated.h"

/**
 * 
 */
UCLASS()
class GOPROJECTEDITOR_API UGoEditorUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoardDataAsset* BoardDataAsset;
	
};
