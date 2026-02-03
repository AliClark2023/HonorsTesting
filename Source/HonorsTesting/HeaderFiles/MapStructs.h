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


// describes surrounding tiles and their corresponding angle values (degrees)
/*USTRUCT(BlueprintType)
struct FTileDirections
{
	GENERATED_BODY()
	FTileDirections()
	{
		Directions.Add(ETileNeighbour::North,FVector2D(0, 60));
		Directions.Add(ETileNeighbour::Northeast,FVector2D(60, 120));
		Directions.Add(ETileNeighbour::Southeast,FVector2D(120, 180));
		Directions.Add(ETileNeighbour::South,FVector2D(180, 240));
		Directions.Add(ETileNeighbour::Southwest,FVector2D(240, 300));
		Directions.Add(ETileNeighbour::Northwest,FVector2D(300, 360));
	}

	ETileNeighbour GetDirection(float Angle)
	{
		for (const auto& Dir : Directions)
		{
			if (Angle >= Dir.Value.X && Angle < Dir.Value.Y) return Dir.Key;
		}
		return ETileNeighbour::North;
	};
private:
	TMap<ETileNeighbour, FVector2D> Directions;
};
*/