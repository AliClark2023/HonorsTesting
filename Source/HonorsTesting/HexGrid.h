// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Components/InstancedStaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "HeaderFiles/MapStructs.h"
#include "Classes/CellularAutomata.h"
#include "Classes/PerlinWorm.h"
#include "GameFramework/Actor.h"
#include "HeaderFiles/TileDirectionUtils.h"
#include "HexGrid.generated.h"

UCLASS()
class HONORSTESTING_API AHexGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
public:
	// functions will construct their respective map segments
	
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ConstructGrid();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateGrid();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GeneratePath();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateLandscape();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void _ClearGrid();
	
	// will require enum to select which method to use
	UFUNCTION(BlueprintCallable, Category = "Generation")
	TArray<FVector> Walker();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void PerlinLandscape();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	TArray<FVector> PerlinPaths();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	TArray<FVector> DiffuseLimited();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	TArray<FVector>  Automata();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void VoronoiRegions();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	FVector ConstructLevel();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	FVector LoadConstruction(FGridProperties GridLayout);
	UFUNCTION(BlueprintCallable, Category = "Grid Calculations")
	void CalculateGrid();
	UFUNCTION(BlueprintCallable, Category = "Query Grid")
	FVector GetEndPoint();
	UFUNCTION(BlueprintCallable, Category = "Query Grid")
	void SetStartPoint(FVector gridPos);


private:
	float _CalculateTileHeight() const;
	void _clearPath();
	// also clears path start point atm
	void _clearLand();
	void _clearRegions() const;
	// moved to Utils file
	/*
	TPair<FVector, bool> GetNeighbour(const ETileNeighbour Neighbour, const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthEastNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthEastNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthWestNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthWestNeighbour(const FVector& CurrentTile) const;
	*/
	//bool TileOnBoundary(const FVector& CurrentTile) const;
	//bool TileBeforeBoundary(const FVector& CurrentTile) const;
	FGameplayTag GetRegionTag(ERegionType Type) const;
	void FinalizePaths(TArray<FVector>& Path);
	void UpdatePaths(TArray<FVector>& Path);
	void UpdateTiles(const TPair<FVector,FGameplayTagContainer>& Tiles);
	//void UpdateStartPoint(FVector& Tile);
	void UpdateTile(FVector& Tile, FGameplayTag TagToAdd);
	// walker algorithms
	static TPair<bool, ETileNeighbour> DrunkardsWalk(const TArray<ETileNeighbour>& VisitedTiles);
public:
	// Map tile properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Config")
	FTileConfig TileConfig;
	
	// Grid Specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties");
	FGridConfig GridConfig;
	
	/*
	 * Algorithms
	 */
	// DW specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FDrunkardConfig DrunkardConfig;
	
	// Perlin Landscape Specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FPerlinLandscapeConfig PerlinLandscapeConfig;
	
	// Perlin path specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FWormConfig PerlinWorms;
	
	// DLA path specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FDlaConfig DlaConfig;
	// Cellular Automata specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FCellularConfig CellularConfig;
	
	//Voronoi Specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms", meta = (ClampMin = "1", ClampMax = "10"))
	int NumberOfRegions = 2;
	
	UPROPERTY(BlueprintReadOnly, Category = "Grid Properties")
	FGridProperties GridInfo;

	// Generation initialisation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialisation")
	FOperationConfig OperationConfig;

private:
	// holds grid coord and its relative index number for instance meshes
	TMap<FVector, int32> LandIndex;
	TMap<FVector, int32> PathIndex;
	TMap<FVector, int32> LavaIndex;
	TMap<FVector, int32> WaterIndex;
	TMap<FVector, int32> MossIndex;
	TMap<FVector, int32> IceIndex;
	TMap<FVector, int32> RockIndex;
	
	
};
