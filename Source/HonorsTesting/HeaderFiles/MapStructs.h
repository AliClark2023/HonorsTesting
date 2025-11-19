#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "GameplayTagContainer.h"
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
	float TileHeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FGameplayTagContainer TileTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	bool Modified = false;
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

UENUM(BlueprintType)
enum class ERegionType : uint8{
	Lava UMETA(DisplayName = "Lava"),
	Water UMETA(DisplayName = "Water"),
	Moss UMETA(DisplayName = "Moss"),
	Ice UMETA(DisplayName = "Ice"),
	Rock UMETA(DisplayName = "Rock"),
};