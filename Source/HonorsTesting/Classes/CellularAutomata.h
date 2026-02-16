#pragma once
#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"
#include "HonorsTesting/HeaderFiles/TileDirectionUtils.h"
#include "HonorsTesting/HeaderFiles/MapStructs.h"
#include "CellularAutomata.generated.h"

// Handles Cellular Automata Rule set functions
UCLASS()
class HONORSTESTING_API UCellularAutomata: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static TPair<bool,FTilePropertiesStruct> Rule30(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, 
		const FVector &StartingTile, const FCellularConfig &CellConfig);
};
