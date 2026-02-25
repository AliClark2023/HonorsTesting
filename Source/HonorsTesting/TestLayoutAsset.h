// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HeaderFiles/MapStructs.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TestLayoutAsset.generated.h"

/**
 * 
 */

USTRUCT()
struct FLayoutInfo {
	GENERATED_BODY()
					 
	UPROPERTY(EditAnywhere)
	float LayerDistance;
	
	UPROPERTY(EditAnywhere)
	FIntVector2 MapSize; 
	
	UPROPERTY(EditAnywhere)
	int PathSize; 
	
	UPROPERTY(EditAnywhere)
	TMap<int, FGridProperties> LayerGridMap;
};

UCLASS()
class HONORSTESTING_API UTestLayoutAsset : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FLayoutInfo Dw_Layout;
	
	UPROPERTY(EditAnywhere)
	FLayoutInfo Pw_Layout;
	
	UPROPERTY(EditAnywhere)
	FLayoutInfo Dla_Layout;
	
	UPROPERTY(EditAnywhere)
	FLayoutInfo Cellular_Layout;
};
