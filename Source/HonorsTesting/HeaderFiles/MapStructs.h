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
	FVector WorldLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FGameplayTag TileStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	float TileHeight;
};

UENUM(BlueprintType)
enum class ETileNeighbour : uint8{
	North UMETA(DisplayName = "North"),
	Northeast UMETA(DisplayName = "NorthEast"),
	Southeast UMETA(DisplayName = "SouthEast"),
	South UMETA(DisplayName = "South"),
	Southwest UMETA(DisplayName = "SouthWest"),
	Northwest UMETA(DisplayName = "NorthWest")
};