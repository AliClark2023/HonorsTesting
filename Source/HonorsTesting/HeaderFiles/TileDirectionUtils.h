#pragma once
#include "CoreMinimal.h"
#include "MapStructs.h"
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
	// tile utilities
	UFUNCTION(BlueprintCallable, Category = "Tiles")
	static ETileNeighbour GetDirectionFromAngle(float Angle);
	// getting tile ( can make parameters a struct for easier readability )
	static TPair<FVector, bool> GetNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const ETileNeighbour& Neighbour, const FVector& CurrentTile);
	static TPair<FVector, bool> NorthNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const FVector& CurrentTile);
	static TPair<FVector, bool> NorthEastNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const FVector& CurrentTile);
	static TPair<FVector, bool> SouthEastNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const FVector& CurrentTile);
	static TPair<FVector, bool> SouthNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const FVector& CurrentTile);
	static TPair<FVector, bool> SouthWestNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const FVector& CurrentTile);
	static TPair<FVector, bool> NorthWestNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const FVector& CurrentTile);
	
	static bool IsTileBeforeBoundary(const int GridColumn, const int GridRow, const FVector& CurrentTile);
	static bool IsTileOnBoundary(const int GridColumn, const int GridRow, const FVector& CurrentTile);
	// cube helper functions (made from https://www.redblobgames.com/grids/hexagons/#conversions)
	static FIntVector CubeSubtract(const FIntVector& A, const FIntVector& B);
	static int CubeDistance(const FIntVector& A, const FIntVector& B);
	static FIntVector EvenQToCube(const FVector& EvenQ);
	static FVector CubeToEvenQ(const FIntVector& Cube);
	static FIntVector CubeRound(const FVector& FracCoords);
	static float Lerp(float PointA, float PointB, float Fraction);
	static FVector CubeLerp(FIntVector PointA, FIntVector PointB, float Fraction);
	// Tiles must be in cube coordinates
	static TArray<FIntVector> CubeLineDraw(const FIntVector& TileA, const FIntVector& TileB);
	
	//  path algorithm utilities
	static TPair<int, TArray<FVector>> CountIslands(TMap<FVector, FTilePropertiesStruct> &GridTiles, FVector2D GridSize, const FGameplayTagContainer &TagsToFind, const FGameplayTag &TagToSet);
	static void JoinIslands(TMap<FVector, FTilePropertiesStruct>& GridTiles, const FGameplayTag& TagToSet, const FGameplayTagContainer &TagsToExclude, const int &Islands, const TArray<FVector>& IslandCentroids);
	static bool IsTileSafe(TMap<FVector, FTilePropertiesStruct> &GridTiles, FVector TileToVisit, TArray<TArray<bool>> &Visited, const FGameplayTagContainer &TagsToFind, FVector2D GridSize);
	static FVector BFS(TMap<FVector, FTilePropertiesStruct> &GridTiles, FVector TileToVisit, TArray<TArray<bool>> &Visited, const FGameplayTagContainer &TagsToFind, FVector2D GridSize);
	static TPair<bool,FVector> FindTile(const TMap<FVector, FTilePropertiesStruct> &GridTiles, const TArray<FVector> &TilesToCheck,  const FGameplayTagContainer &TagsToFind);
	
private:
	// getting tiles direct coords (make private?)
	static FVector GetOppositeNeighbour(ETileNeighbour CurrentNeighbour, const FVector& CurrentTile );
	static FVector NorthNeighbourCoords(const FVector& CurrentTile);
	static FVector NorthEastNeighbourCoords(const FVector& CurrentTile);
	static FVector SouthNeighbourCoords(const FVector& CurrentTile);
	static FVector SouthEastNeighbourCoords(const FVector& CurrentTile);
	static FVector SouthWestNeighbourCoords(const FVector& CurrentTile);
	static FVector NorthWestNeighbourCoords(const FVector& CurrentTile);
};
