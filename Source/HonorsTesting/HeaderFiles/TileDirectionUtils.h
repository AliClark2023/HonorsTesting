#pragma once
#include "CoreMinimal.h"
#include "TileDirectionUtils.generated.h"

// Will include all neighbouring tile calculations and classes

// describing neighbouring tiles
UENUM(BlueprintType)
enum class ETileNeighbour : uint8{
	North UMETA(DisplayName = "North"),
	Northeast UMETA(DisplayName = "NorthEast"),
	Southeast UMETA(DisplayName = "SouthEast"),
	South UMETA(DisplayName = "South"),
	Southwest UMETA(DisplayName = "SouthWest"),
	Northwest UMETA(DisplayName = "NorthWest")
};

UCLASS()
class HONORSTESTING_API UTileDirectionUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Tiles")
	static ETileNeighbour GetDirectionFromAngle(float Angle);
	static FVector NorthNeighbourCoords(const FVector& CurrentTile);
	static FVector NorthEastNeighbourCoords(const FVector& CurrentTile);
	static FVector SouthNeighbourCoords(const FVector& CurrentTile);
	static FVector SouthEastNeighbourCoords(const FVector& CurrentTile);
	static FVector SouthWestNeighbourCoords(const FVector& CurrentTile);
	static FVector NorthWestNeighbourCoords(const FVector& CurrentTile);
};
