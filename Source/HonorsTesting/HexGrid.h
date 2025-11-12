// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Components/InstancedStaticMeshComponent.h"
#include "HeaderFiles/MapStructs.h"
#include "GameFramework/Actor.h"
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
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ConstructGrid();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void _ClearGrid();
	// move algorithms into another cpp file if possible
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void DrunkardsWalk();
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void PerlinLandscape();
	UFUNCTION(BlueprintCallable, Category = "Grid Calculations")
	void CalculateGrid();

	
private:
	float _CalculateTileHeight() const;
	// add map to parameter and make a static const function? then move to calculation header file
	TPair<FVector, bool> NorthNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthEastNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthEastNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> SouthWestNeighbour(const FVector& CurrentTile) const;
	TPair<FVector, bool> NorthWestNeighbour(const FVector& CurrentTile) const;
	
public:
	// map tile properties (change to static meshes?)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float TileRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* landMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	UInstancedStaticMeshComponent* pathMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	int Columns = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	int Rows = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	int PathSize = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	float HeightMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	float NoiseScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	int IterationAttempts = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation Area")
	int CurrentIteration = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Area")
	FVector StartPoint = FVector(0.0f,0.0f,0.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Generation Area")
	TMap<FVector, FTilePropertiesStruct> GridInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bInitialiseGrid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bGenerate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Generation")
	bool bAddHeight;

private:
	//TArray<FVector> _TestPath;
	TMap<FVector, int32> GridToLandInstanceIndex;
	TMap<FVector, int32> GridToPathInstanceIndex;
	
	
};
