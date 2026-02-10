#pragma once
#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Actor.h"
#include "HonorsTesting/HeaderFiles/TileDirectionUtils.h"
#include "HonorsTesting/HeaderFiles/MapStructs.h"
#include "DrunkardWalk.generated.h"

// class will handle Drunkard walks
UCLASS()
class HONORSTESTING_API UDrunkardWalk : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// will stop and return current location when TagToFind has been found
	static TPair<bool,FIntVector2> Walk(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, 
		const FIntVector2 &StartPoint, const FGameplayTagContainer& TagsToFind, const EDlaType& WalkType);
};
