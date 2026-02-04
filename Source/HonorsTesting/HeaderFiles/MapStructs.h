#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "GameplayTagContainer.h"
#include "MapStructs.generated.h"


// describing region types
UENUM(BlueprintType)
enum class ERegionType : uint8{
	Lava UMETA(DisplayName = "Lava"),
	Water UMETA(DisplayName = "Water"),
	Moss UMETA(DisplayName = "Moss"),
	Ice UMETA(DisplayName = "Ice"),
	Rock UMETA(DisplayName = "Rock"),
};

// describes tile properties
USTRUCT(BlueprintType)
struct FTilePropertiesStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FVector WorldLocation;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	//FGameplayTag TileStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	float TileHeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FGameplayTagContainer TileTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	bool Modified = false;
};

// describes grid layouts used in each layer
USTRUCT(BlueprintType)
struct FGridProperties
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridProperties")
	TMap<FVector, FTilePropertiesStruct> GridTiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridProperties")
	FVector StartPoint = FVector(0.0f,0.0f,0.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridProperties")
	FVector EndPoint = FVector(0.0f,0.0f,0.0f);
};


// describe worm characteristics for perlin worms
USTRUCT(BlueprintType)
struct FWormConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "1"))
	int NumWorms = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig")
	int Length = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "0"))
	int OriginalSeed = 12345;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "0"))
	float Freq = 0.1;
	
};