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

// path type selection
UENUM(BlueprintType)
enum class EPathType : uint8
{
	DrunkardWalk UMETA(DisplayName = "DrunkardWalk"),
	PerlinWorm UMETA(DisplayName = "PerlinWorm"),
	DiffuseLimited UMETA(DisplayName = "DiffuseLimited"),
	CellularAutomata UMETA(DisplayNAme = "CellularAutomata"),
};

// DLA type selection
UENUM(BlueprintType)
enum class EDlaType : uint8
{
	Inwards UMETA(DisplayName = "Inwards"),
	Outwards UMETA(DisplayName = "Outwards"),
	Central UMETA(DisplayName = "Central"),
};

// Cellular Automata Rule Selection
UENUM(BlueprintType)
enum class ECellularType : uint8
{
	Rule30 UMETA(DisplayName = "Rule30"),
};

// describes tile properties
USTRUCT(BlueprintType)
struct FTilePropertiesStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FVector WorldLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	float TileHeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	FGameplayTagContainer TileTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileProperties")
	bool Modified = false;
};

// Generation operation
USTRUCT(BlueprintType)
struct FOperationConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operations")
	bool bInitialiseGrid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operations")
	bool bGeneratePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operations")
	bool bGenerateLandscape;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operations")
	bool bGenerateRegions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Operations")
	EPathType PathMethod = EPathType::DrunkardWalk;
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

// Grid specific attributes
USTRUCT(BlueprintType)
struct FGridConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties", meta = (ClampMin = "1", ClampMax = "100"))
	int Columns = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties", meta = (ClampMin = "1", ClampMax = "100"))
	int Rows = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Properties", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	FVector PathStartPoint = FVector(0.0f,0.0f,0.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid Properties", meta = (ClampMin = "1.0", ClampMax = "1000.0"))
	FVector EndPoint = FVector(0.0f,0.0f,0.0f);
};

// Tile Specific attributes
USTRUCT(BlueprintType)
struct FTileConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	float TileRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* landMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* pathMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* pathStartMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* pathEndMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* LavaMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* WaterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* MossMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* IceMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfig")
	UInstancedStaticMeshComponent* RockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag PathTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag LandTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Landscape");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag PathStartTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.PathStart");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag PathEndTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.PathEnd");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag LavaTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Lava");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag IceTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Ice");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag MossTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Moss");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag RockTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Rock");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TileConfigTags")
	FGameplayTag WaterTag = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Regions.Water");
};

/*
 * Algorithm Structs
 */
// Parameters governing Drunkard Walk
USTRUCT(BlueprintType)
struct FDrunkardConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrunkardConfig", meta = (ClampMin = "1", ClampMax = "1000"))
	int PathSize = 5;
	// used to limit algorithm and prevent UE from crashing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrunkardConfig", meta = (ClampMin = "1", ClampMax = "1000"))
	int IterationAttempts = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DrunkardConfig")
	int CurrentIteration = 0;
};
// Parameters governing Perlin Worms
USTRUCT(BlueprintType)
struct FWormConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "1"))
	int NumWorms = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig")
	bool AreIslands = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig")
	int Length = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig")
	bool RandomWormStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "0"))
	int OriginalSeed = 12345;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "0"))
	int WormSeedOffset = 123;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "0", ClampMax = "5"))
	float OriginalFreq = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WormConfig", meta = (ClampMin = "0", ClampMax = "5"))
	FVector2D WormFreqRange = FVector2D(0.1f, 1.2f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WormConfig")
	int NumberOfIslands =0;
};

// Parameters governing Perlin landscape
USTRUCT(BlueprintType)
struct FPerlinLandscapeConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeConfig", meta = (ClampMin = "1.0", ClampMax = "50.0"))
	float HeightMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeConfig", meta = (ClampMin = "0.0", ClampMax = "1"))
	float FeatureScale = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LandscapeConfig", meta = (ClampMin = "0.0", ClampMax = "1000"))
	FVector2D NoiseOffset = FVector2D(0.1, 0.1);
};

USTRUCT(BlueprintType)
struct FDlaConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLA Seed Config")
	int NumWorms = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLA Seed Config")
	int WormLength = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLA Seed Config", meta = (ClampMin = "1.0"))
	int StartingAreaSeed = 12345;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLA Config")
	EDlaType TypeSelection = EDlaType::Inwards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLA Config")
	bool SeedAreaOnly = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLA Config", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	int FloorSize = 25;
	
};

USTRUCT(BlueprintType)
struct FCellularConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cellular Config")
	ECellularType RuleSet = ECellularType::Rule30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cellular Config", meta = (ClampMin = "1.0", ClampMax = "10000.0"))
	int IterationSelection = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cellular Config", meta = (ClampMin = "1.0", ClampMax = "2.0"))
	int NeighbourDistance = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cellular Config")
	FGameplayTagContainer TagsToCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cellular Config")
	FGameplayTag TagToApply;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cellular Config")
	FGameplayTag TagToRevert;
};