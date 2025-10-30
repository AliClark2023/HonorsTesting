#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsModule.h"
#include "MapStructs.generated.h"

USTRUCT(BlueprintType)
struct FTilePropertiesStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FVector RelativeLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FGameplayTag TileStates;
};
