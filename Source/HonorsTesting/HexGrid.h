// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Components/InstancedStaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "HeaderFiles/MapStructs.h"
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
	// add map to parameter and make a static const function? then move to calculation header file
	TPair<FVector, bool> GetNeighbour(const ETileNeighbour Neighbour, const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthEastNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthEastNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthWestNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthWestNeighbour(const FVector& CurrentTile) const;
	bool TileOnBoundary(const FVector& CurrentTile) const;
	FGameplayTag GetRegionTag(ERegionType Type) const;

	// walker algorithms
	static TPair<bool, ETileNeighbour> DrunkardsWalk(const TArray<ETileNeighbour>& VisitedTiles);
	//TPair<bool, ETileNeighbour>  PerlinWorm(const FVector& CurrentTile, const TArray<ETileNeighbour>& VisitedTiles) const;
public:
	// map tile properties (change to static meshes?)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float TileRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* landMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* pathMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* pathStartMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* pathEndMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* LavaMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* WaterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* MossMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* IceMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* RockMesh;

	// move to other file?
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag PathTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag LandTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Landscape");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag PathStartTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.PathStart");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag PathEndTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.PathEnd");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag LavaTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Lava");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag IceTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Ice");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag MossTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Moss");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag RockTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Rock");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Tags")
	FGameplayTag WaterTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Water");
	

	// Grid Specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties");
	FGridConfig GridConfig;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "1", ClampMax = "100"))
	int Columns = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "1", ClampMax = "100"))
	int Rows = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "1", ClampMax = "1000"))
	int PathSize = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	FVector PathStartPoint = FVector(0.0f,0.0f,0.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation Area", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	FVector EndPoint = FVector(0.0f,0.0f,0.0f);
	*/

	// Perlin Landscape Specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "1.0", ClampMax = "50.0"))
	float HeightMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "0.0", ClampMax = "1"))
	float FeatureScale = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "0.0", ClampMax = "1000"))
	FVector2D NoiseOffset = FVector2D(0.1, 0.1);

	/*
	 * Algorithms
	 */
	
	// Perlin path specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FWormConfig PerlinWorms;

	// DW specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algortithms")
	FDrunkardConfig DrunkardConfig;

	//Voronoi Specific
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area", meta = (ClampMin = "1", ClampMax = "10"))
	int NumberOfRegions = 2;
	
	UPROPERTY(BlueprintReadOnly, Category = "Generation Area")
	//TMap<FVector, FTilePropertiesStruct> GridInfo;
	FGridProperties GridInfo;

	// enabling each generation type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bInitialiseGrid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bGeneratePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bGenerateLandscape;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bGenerateRegions;
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
