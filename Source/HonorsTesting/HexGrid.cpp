// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"

#include "DrunkardWalk.h"
#include "GroomVisualizationData.h"
#include "MathUtil.h"
#include "VectorTypes.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialExpressionOperator.h"

// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Attaching components (need meshes to be assigned in editor)
	TileConfig.landMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LandMesh"));
	RootComponent = TileConfig.landMesh;
	TileConfig.pathMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathMesh"));
	TileConfig.pathMesh->SetupAttachment(RootComponent);
	TileConfig.pathStartMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathStartMesh"));
	TileConfig.pathStartMesh->SetupAttachment(RootComponent);
	TileConfig.pathEndMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathEndMesh"));
	TileConfig.pathEndMesh->SetupAttachment(RootComponent);
	TileConfig.LavaMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LavaMesh"));
	TileConfig.LavaMesh->SetupAttachment(RootComponent);
	TileConfig.WaterMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WaterMesh"));
	TileConfig.WaterMesh->SetupAttachment(RootComponent);
	TileConfig.MossMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MossMesh"));
	TileConfig.MossMesh->SetupAttachment(RootComponent);
	TileConfig.IceMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("IceMesh"));
	TileConfig.IceMesh->SetupAttachment(RootComponent);
	TileConfig.RockMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RockMesh"));
	TileConfig.RockMesh->SetupAttachment(RootComponent);
	
	CellularConfig.CodeSequence.Segments.SetNum(8);
}

// Called when the game starts or when spawned
void AHexGrid::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHexGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHexGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// activated in BP
	//ConstructLevel();
	
	// generate regions
	
}

#if WITH_EDITOR
void AHexGrid::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Get the property that was changed
	FName ChangedPropertyName = (PropertyChangedEvent.Property != nullptr) 
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	// Only enforce size if "Segments" was edited
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(AHexGrid, CellularConfig.CodeSequence.Segments))
	{
		// Only SetNum if the size is incorrect (avoid redundant writes)
		if (CellularConfig.CodeSequence.Segments.Num() != 8)
		{
			CellularConfig.CodeSequence.Segments.SetNum(8);
		}
	}
}
#endif

FVector AHexGrid::ConstructLevel()
{	// new method: generating grid depending on tile tags, generation methods update the tags
	// generation methods only effect outcome of tile tags
	if (OperationConfig.bInitialiseGrid)
	{
		// clear any previous instances
		_ClearGrid();
		CalculateGrid();
		GeneratePath();
		GenerateLandscape();
		VoronoiRegions();
		ConstructGrid();
		return GridInfo.EndPoint;
	}else
	{
		_ClearGrid();
		GridInfo.GridTiles.Empty();
		return GridInfo.EndPoint;
	}
}

FVector AHexGrid::LoadConstruction(FGridProperties GridLayout)
{
	GridInfo = GridLayout;
	ConstructGrid();
	return GridInfo.EndPoint;
}

// randomly selects which algorithm to use for its path generation, requires path config parameters to be assigned before calling
FVector AHexGrid::RandomConstruction()
{
	// select algorithm type
	OperationConfig.PathMethod = static_cast<EPathType>(FMath::RandRange(0, StaticEnum<EPathType>()->NumEnums() - 1));
	
	// select algorithm variation
	CalculateGrid();
	GeneratePath();
	GenerateLandscape();
	VoronoiRegions();
	ConstructGrid();
	return GridInfo.EndPoint;
}

void AHexGrid::ConstructGrid()
{
	
	// step method
	for (auto& Element : GridInfo.GridTiles)
	{
		FTransform SpawnTransform;

		//need to account for land/path variations
		if (Element.Value.TileTags.HasTag(TileConfig.LandTag))
		{
			
			if (Element.Value.TileTags.HasTag(TileConfig.PathStartTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.pathStartMesh->AddInstance(SpawnTransform);
				continue;
			}
			if (Element.Value.TileTags.HasTag(TileConfig.PathEndTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.pathEndMesh->AddInstance(SpawnTransform);
				continue;
			}
			
			// region additions
			if (Element.Value.TileTags.HasTag(TileConfig.LavaTag))
			{
				//SpawnTransform.SetScale3D(FVector(1.f,1.f,1.f));
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.LavaMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(TileConfig.WaterTag)){
				//SpawnTransform.SetScale3D(FVector(1.f,1.f,1.f));
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.WaterMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(TileConfig.MossTag)){
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.MossMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(TileConfig.IceTag)){
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.IceMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(TileConfig.RockTag)){
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.RockMesh->AddInstance(SpawnTransform);
			}else if (!LandIndex.Contains(Element.Key))
			{
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
	
				int32 TileIndex = TileConfig.landMesh->AddInstance(SpawnTransform);
				LandIndex.Add(Element.Key, TileIndex);
			}
		}
		else if (Element.Value.TileTags.HasTag(TileConfig.PathTag))
		{
			if (Element.Value.TileTags.HasTag(TileConfig.PathStartTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.pathStartMesh->AddInstance(SpawnTransform);
				continue;
			}
			if (Element.Value.TileTags.HasTag(TileConfig.PathEndTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				TileConfig.pathEndMesh->AddInstance(SpawnTransform);
				continue;
			}
			
			if (!PathIndex.Contains(Element.Key))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				int32 TileIndex = TileConfig.pathMesh->AddInstance(SpawnTransform);
				PathIndex.Add(Element.Key, TileIndex);
			}
		}
	}
	
}

void AHexGrid::GenerateGrid()
{
	if (OperationConfig.bInitialiseGrid)
	{
		CalculateGrid();
		ConstructGrid();
	}else
	{
		_ClearGrid();
	}
}

void AHexGrid::GeneratePath()
{
	// change to allow tile type from algorithm generation
	TArray<FVector> Path;
	
	if (OperationConfig.bInitialiseGrid && OperationConfig.bGeneratePath)
	{
		
		// generates path then adds path tag to specified tiles
		switch (OperationConfig.PathMethod)
		{
		case EPathType::DrunkardWalk:
			Path = Walker();
			break;
		case EPathType::PerlinWorm:
			Path = PerlinPaths();
			break;
		case EPathType::DiffuseLimited:
			Path = DiffuseLimited();
			break;
		case EPathType::CellularAutomata:
			Path = Automata();
			break;
		default:
			Path = Walker();
		}
		
		// Adds all path tags to vectors in array
		FinalizePaths(Path);
		
		
		// only checks surrounding tiles for islands when specified (check needs to be performed last after tags have bee assigned)
		/*
		if (OperationConfig.AreIslands)
		{
			FGameplayTagContainer TagsToSearch;
			TagsToSearch.AddTag(TileConfig.PathTag);
			TagsToSearch.AddTag(TileConfig.PathStartTag);
			TagsToSearch.AddTag(TileConfig.PathEndTag);
			OperationConfig.NumberOfIslands = UTileDirectionUtils::CountIslands(GridInfo.GridTiles,FVector2D(GridConfig.Columns, GridConfig.Rows),TagsToSearch, TileConfig.PathTag );
		}
		*/
		FGameplayTagContainer TagsToSearch;
		TagsToSearch.AddTag(TileConfig.PathTag);
		TagsToSearch.AddTag(TileConfig.PathStartTag);
		TagsToSearch.AddTag(TileConfig.PathEndTag);
		TPair<int, TArray<FVector>> Islands;
		Islands = UTileDirectionUtils::CountIslands(GridInfo.GridTiles,FVector2D(GridConfig.Columns, GridConfig.Rows),TagsToSearch, TileConfig.PathTag);
		OperationConfig.NumberOfIslands = Islands.Key;
		
		if (Islands.Key > 1 && OperationConfig.LinkIslands)
		{
			FGameplayTagContainer ExcludeTags;
			ExcludeTags.AddTag(TileConfig.PathStartTag);
			ExcludeTags.AddTag(TileConfig.PathEndTag);
			
			UTileDirectionUtils::JoinIslands(GridInfo.GridTiles,FVector2D(GridConfig.Columns, GridConfig.Rows), TileConfig.PathTag, ExcludeTags, Islands.Key,Islands.Value);
			Islands = UTileDirectionUtils::CountIslands(GridInfo.GridTiles,FVector2D(GridConfig.Columns, GridConfig.Rows),TagsToSearch, TileConfig.PathTag);
			OperationConfig.NumberOfIslands = Islands.Key;
			// need way to check for start/end tags and add if not found (not needed as these are excluded in the join islands function)
		}
	}
}

void AHexGrid::GenerateLandscape()
{
	if (OperationConfig.bInitialiseGrid && OperationConfig.bGenerateLandscape)
	{
		PerlinLandscape();
	}else
	{
		for (auto& tiles : LandIndex)
		{
			//need check for valid transform
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(GridInfo.GridTiles.Find(tiles.Key)->WorldLocation);
			TileConfig.landMesh->UpdateInstanceTransform(tiles.Value, SpawnTransform);
		}
	}
	
}

// Walker algorithm that changes its neighbour acquisition method depending on type selected( DW or PW)
//TArray<FVector> AHexGrid::Walker(Selection name)
TArray<FVector> AHexGrid::Walker()
{
	int _attempts = 0;
	TArray<FVector> IteratedPath;
	TArray<FVector> GeneratedPath;
	bool _generate = true;
	
	while (_attempts <= DrunkardConfig.IterationAttempts && _generate)
	{
		FVector _CurrentTile;
		IteratedPath.Empty();
		int _CurrentSteps = 0;
		_attempts++;
		
		if (GridInfo.GridTiles.Contains(GridInfo.StartPoint))
		{
			_CurrentTile = GridInfo.StartPoint;
			IteratedPath.Add(_CurrentTile);
			_CurrentSteps++;
			TArray<ETileNeighbour> _VisitedTiles;
			bool _pathBlocked = false;
			
			while (_CurrentSteps < DrunkardConfig.PathSize && !_pathBlocked)
			{
				// DW method
				TPair<bool, ETileNeighbour> NeighbourTile = DrunkardsWalk(_VisitedTiles);
				ETileNeighbour _ChosenNeighbour = NeighbourTile.Value;
				
				if (NeighbourTile.Key)
				{
					_pathBlocked = true;
					break;
				}
				
				TPair<FVector, bool> _Neighbour;

				switch (_ChosenNeighbour)
				{
				case ETileNeighbour::North:
					_Neighbour = UTileDirectionUtils::NorthNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), _CurrentTile);
					if (_Neighbour.Value && !IteratedPath.Contains(_Neighbour.Key))
					{
						IteratedPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						// add to visited tiles
						// if all tiles have been visited end loop
						_VisitedTiles.Add(ETileNeighbour::North);
					}
					break;
				case ETileNeighbour::Northeast:
					_Neighbour = UTileDirectionUtils::NorthEastNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), _CurrentTile);
					if (_Neighbour.Value && !IteratedPath.Contains(_Neighbour.Key))
					{
						IteratedPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Northeast);
					}
					break;
				case ETileNeighbour::Southeast:
					_Neighbour = UTileDirectionUtils::SouthEastNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), _CurrentTile);
					if (_Neighbour.Value && !IteratedPath.Contains(_Neighbour.Key))
					{
						IteratedPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Southeast);
					}
					break;
				case ETileNeighbour::South:
					_Neighbour = UTileDirectionUtils::SouthNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), _CurrentTile);
					if (_Neighbour.Value && !IteratedPath.Contains(_Neighbour.Key))
					{
						IteratedPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::South);
					}
					break;
				case ETileNeighbour::Southwest:
					_Neighbour = UTileDirectionUtils::SouthWestNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), _CurrentTile);
					if (_Neighbour.Value && !IteratedPath.Contains(_Neighbour.Key))
					{
						IteratedPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Southwest);
					}
					break;
				case ETileNeighbour::Northwest:
					_Neighbour = UTileDirectionUtils::NorthWestNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), _CurrentTile);
					if (_Neighbour.Value && !IteratedPath.Contains(_Neighbour.Key))
					{
						IteratedPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Northwest);
					}
					break;
				}

				if (IteratedPath.Num() >= DrunkardConfig.PathSize)
				{
					// stop loops, successful path generation
					_generate = false;
					break;
				}
			}
		}
		
		// keep generation of greater size while iterating
		if (IteratedPath.Num() >= GeneratedPath.Num()) GeneratedPath = IteratedPath;
	}
	
	DrunkardConfig.CurrentIteration = _attempts;
	return GeneratedPath;
}

void AHexGrid::PerlinLandscape()
{
	// need to find method & parameters to adjust noise value

	for (auto& Tile : GridInfo.GridTiles)
	{
	
		if (Tile.Value.TileTags.HasTag(TileConfig.LandTag))
		{
			const float Frequency =  1.0f / PerlinLandscapeConfig.FeatureScale;
			// float NoiseValue = FMath::PerlinNoise2D(FVector2D(Tile.Key.X * NoiseScale + 0.1, Tile.Key.Y * NoiseScale + 0.1))
			float NoiseValue = FMath::PerlinNoise2D(FVector2D(Tile.Key.X , Tile.Key.Y ) * Frequency + PerlinLandscapeConfig.NoiseOffset );
			
			// normalising, since above function returns a value from -1 to 1
			NoiseValue = (NoiseValue + 1) / 2;

			// adding to scale
			NoiseValue = NoiseValue * PerlinLandscapeConfig.HeightMultiplier;
			Tile.Value.TileHeight = NoiseValue;
			
		}
	}
}
// used perlin worms to create paths
TArray<FVector> AHexGrid::PerlinPaths()
{
	TArray<FPerlinWorm> Worms;
	
	FVector2D WormSP(GridInfo.StartPoint.X, GridInfo.StartPoint.Y);
	float Angle = 0.0f;
	
	for (int i = 0; i < PerlinWorms.NumWorms; i++)
	{
		//need to adjust seed, freq and other variables to get variations for each worm.
		FPerlinWorm TestWorm(WormSP, TileConfig.PathTag);
		// random coords
		const float WormSeed = PerlinWorms.OriginalSeed + i * PerlinWorms.WormSeedOffset;
		const float WormFreq = PerlinWorms.OriginalFreq + i * FMath::RandRange(PerlinWorms.WormFreqRange.X, PerlinWorms.WormFreqRange.Y);
		
		
		// testing (for joining islands)
		//const float WormSeed = PerlinWorms.OriginalSeed + i;
		//const float WormFreq = PerlinWorms.OriginalFreq + i;
		
		for (int j = 0; j < PerlinWorms.Length; j++)
		{
			float Noise = FMath::PerlinNoise2D(FVector2D((TestWorm.GetX() + WormSeed) * WormFreq,
				(TestWorm.GetY() + WormSeed) * WormFreq));
			// normalising (0 to 1) then applying it to degrees
			Angle = ((Noise + 1) / 2) * 360;
		
			ETileNeighbour Dir = UTileDirectionUtils::GetDirectionFromAngle(Angle);
			TPair<FVector,bool> Neighbour = UTileDirectionUtils::GetNeighbour(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows),
				Dir,FVector(TestWorm.GetX(), TestWorm.GetY(), 0));

			// Valid grid coord check
			
			if (Neighbour.Value)
			{
				if (TestWorm.Display().Contains(FVector2D(Neighbour.Key.X, Neighbour.Key.Y))) break;
				FVector2D StepPos = FVector2D(Neighbour.Key.X, Neighbour.Key.Y);
				TestWorm.Grow(StepPos);
			}else
			{
				break;
			}
			
		}
		Worms.Add(TestWorm);
		// need to modify starting point of next worm
		if (PerlinWorms.RandomWormStart)
		{
			// random starting point within grid (Produces best result but with no guarantee that path is contiguous)
			WormSP.X = FMath::RandRange(1,GridConfig.Columns - 2);
			WormSP.Y = FMath::RandRange(1,GridConfig.Rows - 2);

			// testing (removing randomness to test island links)
			//WormSP.X = GridConfig.Columns / 2;
			//WormSP.Y = GridConfig.Rows / 2;
		}else
		{
			// starting new worm at random point of previous worm
			WormSP = TestWorm.GetPointOnSegment();
		}
		

		
	}
	
	// updating grid info with worm locations
	TArray<FVector> WormPaths;
	
	for (int i = 0; i < Worms.Num(); i++)
	{
		TArray<FVector2D> WormCoords = Worms[i].Display();
		
		for (int j = 0; j < WormCoords.Num(); j++)
		{
			WormPaths.Push(FVector(WormCoords[j], 0));
		}
	}
	return WormPaths;
}

TArray<FVector> AHexGrid::DiffuseLimited()
{
	// creating initial seed area from start point

	// using 1 perlin worm to create seed area, transfer seed number to perlin config
	PerlinWorms.OriginalSeed = DlaConfig.StartingAreaSeed;
	PerlinWorms.Length = DlaConfig.WormLength;
	PerlinWorms.NumWorms = DlaConfig.NumWorms;
	// this needs to set tiles tags in grid before walks begin
	const TArray<FVector> SeedArea = PerlinPaths();
	TArray<FVector> FloorPlan = SeedArea;
	UpdatePaths(FloorPlan);
	
	// only showing seed area (for visualising starting area)
	if (DlaConfig.SeedAreaOnly) return FloorPlan;
	
	// create walkers until specified floor size has been met
	int CurrentFloorSize = FloorPlan.Num();
	
	while (CurrentFloorSize < DlaConfig.FloorSize)
	{
		// where to spawn walker
		FIntVector2 WalkerSpawn = FIntVector2(0,0);
		TPair<bool, FIntVector2> WalkResult = TPair<bool,FIntVector2>(false,FIntVector2(0,0));
		// Tiles to search for
		FGameplayTagContainer TagsToFind;
		
		
		switch (DlaConfig.TypeSelection)
		{
		case EDlaType::Inwards:
			// tags to search for
			TagsToFind.Reset();
			TagsToFind.AddTag(TileConfig.PathTag);
			TagsToFind.AddTag(TileConfig.PathStartTag);
			TagsToFind.AddTag(TileConfig.PathEndTag);
			// spawn walkers at random points within the grid (and boundary), if it hits path, previous tile becomes path
			WalkerSpawn.X = FMath::RandRange(1,GridConfig.Columns - 2);
			WalkerSpawn.Y = FMath::RandRange(1,GridConfig.Rows - 2);
			WalkResult = UDrunkardWalk::Walk(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows),
				WalkerSpawn, TagsToFind, DlaConfig.TypeSelection);
			
			// specified tile found, update floor plan, update grid
			if (WalkResult.Key)
			{
				FVector TileToUpdate = FVector(WalkResult.Value.X, WalkResult.Value.Y, 0);
				FloorPlan.Add(FVector(WalkResult.Value.X, WalkResult.Value.Y, 0));
				
				UpdateTile(TileToUpdate,TileConfig.PathTag);
				
				CurrentFloorSize ++;
			}
			
			break;
			// creates radial open-plan area, not much variation (except on the borders) not particularly suited for hex grids
			case EDlaType::Outwards:
			// Tile to search for
			TagsToFind.Reset();
			TagsToFind.AddTag(TileConfig.LandTag);
			
			// spawn walkers within created path then walk outwards
			FVector TileToSpawn = FloorPlan[FMath::RandRange(0,FloorPlan.Num() - 1)];
			//WalkerSpawn = FIntVector2(GridInfo.StartPoint.X,GridInfo.StartPoint.Y);
			WalkerSpawn = FIntVector2(TileToSpawn.X,TileToSpawn.Y);
			
			
			WalkResult = UDrunkardWalk::Walk(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows),
				WalkerSpawn, TagsToFind, DlaConfig.TypeSelection);
			
			// specified tile found, update floor plan, update grid
			if (WalkResult.Key)
			{
				FVector TileToUpdate = FVector(WalkResult.Value.X, WalkResult.Value.Y, 0);
				FloorPlan.Add(FVector(WalkResult.Value.X, WalkResult.Value.Y, 0));
				
				UpdateTile(TileToUpdate,TileConfig.PathTag);
				
				CurrentFloorSize ++;
			}
			
			break;
		default: // Central
			// spawn walkers at random location within grid
			// plot line from current location to central point
			// traverse line, if it hits path, previous tile also becomes path.
			// tags to search for
			TagsToFind.Reset();
			TagsToFind.AddTag(TileConfig.PathTag);
			TagsToFind.AddTag(TileConfig.PathStartTag);
			TagsToFind.AddTag(TileConfig.PathEndTag);
			
			WalkerSpawn.X = FMath::RandRange(1,GridConfig.Columns - 2);
			WalkerSpawn.Y = FMath::RandRange(1,GridConfig.Rows - 2);
			FVector CentralPoint = GridInfo.StartPoint;
			FVector WalkerSP = FVector(WalkerSpawn.X,WalkerSpawn.Y, 0);
			
			FIntVector CentralPointCube = UTileDirectionUtils::EvenQToCube(CentralPoint);
			FIntVector WalkerSPCube = UTileDirectionUtils::EvenQToCube(WalkerSP);
			
			TArray<FIntVector> CubedTiles = UTileDirectionUtils::CubeLineDraw(WalkerSPCube,CentralPointCube);
			
			TArray<FVector> TilesToCheck;
			for (auto& Tile : CubedTiles)
			{
				//need to convert back to Even Q
				FVector EvenQTile = UTileDirectionUtils::CubeToEvenQ(Tile);
				TilesToCheck.Add(EvenQTile);
			}
			
			TPair<bool, FVector> PossibleTile = UTileDirectionUtils::FindTile( GridInfo.GridTiles, TilesToCheck, TagsToFind);
			if (PossibleTile.Key)
			{
				FloorPlan.Add(FVector(PossibleTile.Value.X, PossibleTile.Value.Y, 0));
				
				UpdateTile(PossibleTile.Value,TileConfig.PathTag);
				
				CurrentFloorSize ++;
			}
			
			break;
		}
	}
	return FloorPlan;
}
// checks each tile and updates their state (Tags) based on the rule set selected
TArray<FVector>  AHexGrid::Automata()
{
	// populate grid with random tiles (update grid tiles)
	TArray<FVector> UpdatedTiles;
	TArray<TPair<FVector,FGameplayTagContainer>> TilesToUpdate;
	
	//if (CellularConfig.RuleSet == ECellularType::GameOfLife)
	if (CellularConfig.PopulateTiles)
	{
		UpdatedTiles = UCellularAutomata::RandomPopulate(GridInfo.GridTiles, FIntVector2(GridConfig.Columns,GridConfig.Rows), CellularConfig.GameOfLifeConfig.ChanceToStartAlive);
		UpdatePaths(UpdatedTiles);
	}
	
	UpdatedTiles.Reset();
	
	// perform Automata depending on selection
	for (int i = 0; i <CellularConfig.IterationSelection; i ++)
	{
		for (auto& Tile: GridInfo.GridTiles)
		{
			if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows, Tile.Key))  continue;
			
			TPair<bool, FTilePropertiesStruct> NewState;
		
			switch (CellularConfig.RuleSet)
			{
			case ECellularType::Rule30:
				NewState = UCellularAutomata::Rule30(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), Tile.Key,CellularConfig);
				break;
			case ECellularType::Wolfram:
				NewState = UCellularAutomata::Wolfram(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), Tile.Key,CellularConfig);
				break;
			case ECellularType::GameOfLife:
				NewState = UCellularAutomata::GameOfLife(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), Tile.Key,CellularConfig);
				break;
			default:
				NewState = UCellularAutomata::Rule30(GridInfo.GridTiles, FIntVector2(GridConfig.Columns, GridConfig.Rows), Tile.Key,CellularConfig);
				break;
			}
		
			if (!NewState.Key) continue;
			if (Tile.Value.TileTags != NewState.Value.TileTags)
			{
				//UpdatedTiles.Add(Tile.Key);
				
				// add grid coord and tag type to update
				TilesToUpdate.Add(TPair<FVector,FGameplayTagContainer>(Tile.Key,NewState.Value.TileTags));
			}
		}
		
		
		
		//update tiles after each iteration pass
		/*
		for (auto& Tile: UpdatedTiles)
		{
			GridInfo.GridTiles.Add(Tile);
		}
		*/
		for (auto& Tile : TilesToUpdate)
		{
			// new update tile function
			UpdateTiles(Tile);
		}
		TilesToUpdate.Reset();
		
		/*
		UpdatePaths(UpdatedTiles);
		UpdatedTiles.Reset();
		*/
	}
	
	// returns all path tiles to main function for processing start and end points
	TArray<FVector> PathTiles;
	for (auto& Tile: GridInfo.GridTiles)
	{
		if (Tile.Value.TileTags.HasAny(CellularConfig.TagsToCheck)) PathTiles.Add(Tile.Key);
	}
	
	return PathTiles;
}

void AHexGrid::VoronoiRegions()
{
	if (!OperationConfig.bGenerateRegions) return;
	
	//TArray<FVector> _Regions;
	TMap<FVector, ERegionType> Regions;
	FVector NewCoord;

	const int MaxRegions = StaticEnum<ERegionType>()->NumEnums() - 2;
	

	// Generating voronoi seed points (starting points for regions) (grid coord)
	/*
	while (!Regions.Contains(NewCoord) && Regions.Num() < NumberOfRegions)
	{
		NewCoord.X = FMath::RandRange(0,Columns);
		NewCoord.Y = FMath::RandRange(0,Rows);
		ERegionType ChosenRegion = static_cast<ERegionType>(FMath::RandRange(0, MaxRegions));
		
		Regions.Add(NewCoord, ChosenRegion);
	}
	*/
	
	for (int i = 0; i < NumberOfRegions; i++)
	{
		NewCoord.X = FMath::RandRange(0,GridConfig.Columns);
		NewCoord.Y = FMath::RandRange(0,GridConfig.Rows);
		ERegionType ChosenRegion = static_cast<ERegionType>(FMath::RandRange(0, MaxRegions));
		
		Regions.Add(NewCoord, ChosenRegion);
	}
	

	// assigning region to tile based on shortest distance from tile to seed point
	for (auto& Tile : GridInfo.GridTiles)
	{
		FVector TilePos = Tile.Key;
		FVector RegionPos;
		float Distance = 1000000.f;

		// determining coord of shortest distance
		for (auto& Region : Regions)
		{
			float CurrentDist = FVector::Dist(TilePos, Region.Key);
			if (CurrentDist < Distance)
			{
				Distance = CurrentDist;
				RegionPos = Region.Key;
			}
		}

		// add region tag to tile
		//Tile.Value.TileStates = GetRegionTag( *Regions.Find(RegionPos));
		FGameplayTag tag = GetRegionTag( *Regions.Find(RegionPos));

		if (tag != FGameplayTag::EmptyTag) Tile.Value.TileTags.AddLeafTag(tag);
	}
}



void AHexGrid::CalculateGrid()
{
	if (!GridInfo.GridTiles.IsEmpty()) GridInfo.GridTiles.Empty();
	
	for (int y = 0; y < GridConfig.Rows; y++)
	{
		for (int x = 0; x < GridConfig.Columns; x++)
		{
			if (x % 2 == 0)
			{
				// even hex tiles & their respective calculations
				FVector GridCoord = FVector(x, y, 0.0f);
				FVector TileLocation;
				TileLocation.X = _CalculateTileHeight() * y;
				TileLocation.Y = (TileConfig.TileRadius * 2) * 0.75 * x;
				TileLocation.Z = 0;

				FTilePropertiesStruct Tile;
				Tile.WorldLocation = TileLocation;
				//Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				//Tile.TileStates = LandTag;
				Tile.TileTags.AddTag(TileConfig.LandTag);

				GridInfo.GridTiles.Add(GridCoord, Tile);
			}
			else
			{
				// odd hex tiles & their respective calculations
				FVector GridCoord = FVector(x, y, 0.0f);
				FVector TileLocation;
				TileLocation.X = (_CalculateTileHeight() * y) + (_CalculateTileHeight() / 2);
				TileLocation.Y = (TileConfig.TileRadius * 2) * 0.75 * x;
				TileLocation.Z = 0;

				FTilePropertiesStruct Tile;
				Tile.WorldLocation = TileLocation;
				//Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				//Tile.TileStates = LandTag;
				Tile.TileTags.AddTag(TileConfig.LandTag);

				GridInfo.GridTiles.Add(GridCoord, Tile);
			}
		}
	}

	//if (GridInfo.Contains(StartPoint)) GridInfo[StartPoint].TileStates = PathStartTag;
	if (GridInfo.GridTiles.Contains(GridInfo.StartPoint))
	{
		GridInfo.GridTiles[GridInfo.StartPoint].TileTags.AddLeafTag(TileConfig.PathStartTag);
	}
}

FVector AHexGrid::GetEndPoint()
{
	return GridInfo.EndPoint;
}

void AHexGrid::SetStartPoint(FVector gridPos)
{
	GridInfo.StartPoint = gridPos;
}

/*
TPair<FVector, bool> AHexGrid::GetNeighbour(const ETileNeighbour Neighbour, const FVector& CurrentTile) const
{
	switch (Neighbour)
	{
	case ETileNeighbour::North:
		return  NorthNeighbour(CurrentTile);
	case ETileNeighbour::Northeast:
		return  NorthEastNeighbour(CurrentTile);
	case ETileNeighbour::Southeast:
		return  SouthEastNeighbour(CurrentTile);
	case ETileNeighbour::South:
		return  SouthNeighbour(CurrentTile);
	case ETileNeighbour::Southwest:
		return  SouthWestNeighbour(CurrentTile);
	case ETileNeighbour::Northwest:
		return  NorthWestNeighbour(CurrentTile);
	default:
		return TPair<FVector, bool>(CurrentTile, false);
	}
}

// calculations assume Even-Q hex grid
TPair<FVector, bool> AHexGrid::NorthNeighbour(const FVector& CurrentTile) const
{
	constexpr ETileNeighbour Type = ETileNeighbour::North;
	FVector TileNeighbour(UTileDirectionUtils::NorthNeighbourCoords(CurrentTile));
	
	if (!GridInfo.GridTiles.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows, TileNeighbour))
	{
		return TPair<FVector, bool>(UTileDirectionUtils::GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
	
}

TPair<FVector, bool> AHexGrid::NorthEastNeighbour(const FVector& CurrentTile) const
{
	constexpr ETileNeighbour Type = ETileNeighbour::Northeast;
	FVector TileNeighbour(UTileDirectionUtils::NorthEastNeighbourCoords(CurrentTile));
	
	if (!GridInfo.GridTiles.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows, TileNeighbour))
	{
		return TPair<FVector, bool>(UTileDirectionUtils::GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
	
}

TPair<FVector, bool> AHexGrid::SouthEastNeighbour(const FVector& CurrentTile) const
{
	constexpr ETileNeighbour Type = ETileNeighbour::Southeast;
	FVector TileNeighbour(UTileDirectionUtils::SouthEastNeighbourCoords(CurrentTile));


	if (!GridInfo.GridTiles.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows, TileNeighbour))
	{
		return TPair<FVector, bool>(UTileDirectionUtils::GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
	
}

TPair<FVector, bool> AHexGrid::SouthNeighbour(const FVector& CurrentTile) const
{
	constexpr ETileNeighbour Type = ETileNeighbour::South;
	FVector TileNeighbour(UTileDirectionUtils::SouthNeighbourCoords(CurrentTile));
	
	if (!GridInfo.GridTiles.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows, TileNeighbour))
	{
		return TPair<FVector, bool>(UTileDirectionUtils::GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
	
}

TPair<FVector, bool> AHexGrid::SouthWestNeighbour(const FVector& CurrentTile) const
{
	constexpr ETileNeighbour Type = ETileNeighbour::Southwest;
	FVector TileNeighbour(UTileDirectionUtils::SouthWestNeighbourCoords(CurrentTile));

	if (!GridInfo.GridTiles.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows,TileNeighbour))
	{
		return TPair<FVector, bool>(UTileDirectionUtils::GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

TPair<FVector, bool> AHexGrid::NorthWestNeighbour(const FVector& CurrentTile) const
{
	constexpr ETileNeighbour Type = ETileNeighbour::Northwest;
	FVector TileNeighbour(UTileDirectionUtils::NorthWestNeighbourCoords(CurrentTile));
	
	if (!GridInfo.GridTiles.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (UTileDirectionUtils::IsTileOnBoundary(GridConfig.Columns, GridConfig.Rows, TileNeighbour))
	{
		return TPair<FVector, bool>(UTileDirectionUtils::GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
	
}
*/
/*
bool AHexGrid::TileOnBoundary(const FVector& CurrentTile) const
{
	if (CurrentTile.X == 0 || CurrentTile.X == GridConfig.Columns - 1) return true;
	if (CurrentTile.Y == 0 || CurrentTile.Y == GridConfig.Rows - 1) return true;

	return false;
	
}

bool AHexGrid::TileBeforeBoundary(const FVector& CurrentTile) const
{
	if (CurrentTile.X <= 1 || CurrentTile.X >= GridConfig.Columns - 2) return true;
	if (CurrentTile.Y <= 1 || CurrentTile.Y >= GridConfig.Rows - 2) return true;

	return false;
	
}
*/
FGameplayTag AHexGrid::GetRegionTag(const ERegionType Type) const
{
	switch (Type)
	{
		case ERegionType::Lava:
			return TileConfig.LavaTag;
		case ERegionType::Water:
			return TileConfig.WaterTag;
		case ERegionType::Moss:
			return TileConfig.MossTag;
		case ERegionType::Ice:
			return TileConfig.IceTag;
		case ERegionType::Rock:
			return TileConfig.RockTag;
		default:
			return FGameplayTag::EmptyTag;
	}
}

// used to set start and end points of paths
void AHexGrid::FinalizePaths(TArray<FVector>& Path)
{
	for (FVector Element : Path)
	{
		if (FTilePropertiesStruct* TileStatus = GridInfo.GridTiles.Find(Element))
		{
			FTilePropertiesStruct NewStatus;
			NewStatus.WorldLocation = TileStatus->WorldLocation;
			//NewStatus.TileStates = PathTag;
			NewStatus.TileTags.Reset();
			// need duplicate?
			NewStatus.TileTags.AddTag(TileConfig.PathTag);
			
			// marking start and end points of path
			if (Element == Path[0])
			{
				NewStatus.TileTags.AddTag(TileConfig.PathStartTag);
				// updating starting point of path
				GridInfo.StartPoint = Element;
			}else if (Element == (Path[Path.Num()-1])){
				NewStatus.TileTags.AddTag(TileConfig.PathEndTag);
				//EndPoint = Element;
				GridInfo.EndPoint = Element;
			}else
			{
				NewStatus.TileTags.AddTag(TileConfig.PathTag);
			}
				
				
			GridInfo.GridTiles.Add(Element, NewStatus);
		}
	}
}

// updates paths with new values, used before finalizing paths or with any intermediate generations that depend on grid state
void AHexGrid::UpdatePaths(TArray<FVector>& Path)
{
	for (FVector Element : Path)
	{
		if (FTilePropertiesStruct* TileStatus = GridInfo.GridTiles.Find(Element))
		{
			FTilePropertiesStruct NewStatus;
			NewStatus.WorldLocation = TileStatus->WorldLocation;
			//NewStatus.TileStates = PathTag;
			NewStatus.TileTags.Reset();
			NewStatus.TileTags.AddTag(TileConfig.PathTag);
			
			//updating tile
			GridInfo.GridTiles.Add(Element, NewStatus);
		}
	}
}

void AHexGrid::UpdateTiles(const TPair<FVector, FGameplayTagContainer>& Tiles)
{
	FTilePropertiesStruct* FoundTile = GridInfo.GridTiles.Find(Tiles.Key);
	if (FoundTile)
	{
		FTilePropertiesStruct NewStatus;
		NewStatus.WorldLocation = FoundTile->WorldLocation;
		NewStatus.TileTags.Reset();
		NewStatus.TileTags.AppendTags(Tiles.Value);
		
		//updating tile
		GridInfo.GridTiles.Add(Tiles.Key, NewStatus);
	}
}

void AHexGrid::UpdateTile(FVector& Tile, FGameplayTag TagToAdd)
{
	FTilePropertiesStruct* FoundTile = GridInfo.GridTiles.Find(Tile);
	if (FoundTile)
	{
		FTilePropertiesStruct NewStatus;
		NewStatus.WorldLocation = FoundTile->WorldLocation;
		NewStatus.TileTags.Reset();
		NewStatus.TileTags.AddTag(TagToAdd);
		
		//updating tile
		GridInfo.GridTiles.Add(Tile, NewStatus);
	}
}

/*
void AHexGrid::UpdateStartPoint(FVector& Tile)
{
	FVector PrevStartCoord = GridInfo.StartPoint;
	FTilePropertiesStruct*  PrevStartTile = GridInfo.GridTiles.Find(PrevStartCoord);
	
	if (PrevStartTile)
	{
		FTilePropertiesStruct NewStatus;
		NewStatus.WorldLocation = PrevStartTile->WorldLocation;
		NewStatus.TileTags.Reset();
		NewStatus.TileTags.AddTag(TileConfig.LandTag);
		
		//updating tile
		GridInfo.GridTiles.Add(PrevStartCoord, NewStatus);
	}
	
	FVector NewStartCoord = Tile;
	FTilePropertiesStruct*  NewStartTile = GridInfo.GridTiles.Find(PrevStartCoord);
	if (PrevStartTile)
	{
		FTilePropertiesStruct NewStatus;
		NewStatus.WorldLocation = NewStartTile->WorldLocation;
		NewStatus.TileTags.Reset();
		NewStatus.TileTags.AddTag(TileConfig.PathStartTag);
		
		//updating tile
		GridInfo.GridTiles.Add(NewStartCoord, NewStatus);
	}
}
*/

// returns blocked status when visiting neighbours
TPair<bool, ETileNeighbour> AHexGrid::DrunkardsWalk(const TArray<ETileNeighbour>& VisitedTiles)
{
	// DW method
	const int MaxChoice = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
	ETileNeighbour ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, MaxChoice));
	bool PathBlocked = false;			

	// re-selects another neighbour if already visited
	while (VisitedTiles.Contains(ChosenNeighbour))
	{
		if (VisitedTiles.Num() == MaxChoice)
		{
			// break current iteration
			PathBlocked = true;
			break;
		}else
		{
			ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, MaxChoice));
		}
	}
	const TPair<bool, ETileNeighbour> Result(PathBlocked,ChosenNeighbour);
	return Result;
}

// old function not in use
/*
TPair<bool, ETileNeighbour> AHexGrid::PerlinWorm(const FVector& CurrentTile, const TArray<ETileNeighbour>& VisitedTiles) const
{
	const int MaxChoice = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
	bool PathBlocked = false;
	float WormNoise = FMath::PerlinNoise2D(FVector2D((CurrentTile.X + PerlinSeed) * PerlinFreq, (CurrentTile.Y + PerlinSeed) * PerlinFreq));
	// normalizing then multiplying by 360 to get direction angle
	float DirAngle = ((WormNoise + 1) / 2) * 360;
	ETileNeighbour ChosenNeighbour = UTileDirectionUtils::GetDirectionFromAngle(DirAngle);

	// re-selects another neighbour if already visited
	while (VisitedTiles.Contains(ChosenNeighbour))
	{
		WormNoise = FMath::PerlinNoise2D(FVector2D((CurrentTile.X + PerlinSeed) * PerlinFreq, (CurrentTile.Y + PerlinSeed) * PerlinFreq));
		// normalizing then multiplying by 360 to get direction angle
		DirAngle = ((WormNoise + 1) / 2) * 360;
		ETileNeighbour NewNeighbour = UTileDirectionUtils::GetDirectionFromAngle(DirAngle);

		if (VisitedTiles.Num() == MaxChoice || NewNeighbour == ChosenNeighbour)
		{
			// break current iteration
			PathBlocked = true;
			break;
		}else
		{
			ChosenNeighbour = NewNeighbour;
		}
	}
	const TPair<bool, ETileNeighbour> Result(PathBlocked,ChosenNeighbour);
	return Result;
}
*/

float AHexGrid::_CalculateTileHeight() const
{
	return (TileConfig.TileRadius * sqrt(3));
}

void AHexGrid::_clearPath()
{
	TileConfig.pathMesh->ClearInstances();
	PathIndex.Empty();
}

void AHexGrid::_clearLand()
{
	TileConfig.pathStartMesh->ClearInstances();
	TileConfig.pathEndMesh->ClearInstances();
	TileConfig.landMesh->ClearInstances();
	LandIndex.Empty();
}

void AHexGrid::_clearRegions() const
{
	TileConfig.LavaMesh->ClearInstances();
	TileConfig.WaterMesh->ClearInstances();
	TileConfig.MossMesh->ClearInstances();
	TileConfig.IceMesh->ClearInstances();
	TileConfig.RockMesh->ClearInstances();
}

void AHexGrid::_ClearGrid()
{
	_clearPath();
	_clearLand();
	_clearRegions();
}
