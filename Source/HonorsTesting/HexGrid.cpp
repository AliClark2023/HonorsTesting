// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"
#include "MathUtil.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Attaching components (need meshes to be assigned in editor)
	landMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LandMesh"));
	RootComponent = landMesh;
	pathMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathMesh"));
	pathMesh->SetupAttachment(RootComponent);
	pathStartMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathStartMesh"));
	pathStartMesh->SetupAttachment(RootComponent);
	pathEndMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathEndMesh"));
	pathEndMesh->SetupAttachment(RootComponent);
	LavaMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LavaMesh"));
	LavaMesh->SetupAttachment(RootComponent);
	WaterMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WaterMesh"));
	WaterMesh->SetupAttachment(RootComponent);
	MossMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MossMesh"));
	MossMesh->SetupAttachment(RootComponent);
	IceMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("IceMesh"));
	IceMesh->SetupAttachment(RootComponent);
	RockMesh= CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RockMesh"));
	RockMesh->SetupAttachment(RootComponent);
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
FVector AHexGrid::ConstructLevel()
{	// new method: generating grid depending on tile tags, generation methods update the tags
	// generation methods only effect outcome of tile tags
	if (bInitialiseGrid)
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

void AHexGrid::ConstructGrid()
{
	
	// step method
	for (auto& Element : GridInfo.GridTiles)
	{
		FTransform SpawnTransform;

		//need to account for land/path variations
		if (Element.Value.TileTags.HasTag(LandTag))
		{
			
			if (Element.Value.TileTags.HasTag(PathStartTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				pathStartMesh->AddInstance(SpawnTransform);
				continue;
			}
			if (Element.Value.TileTags.HasTag(PathEndTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				pathEndMesh->AddInstance(SpawnTransform);
				continue;
			}
			
			// region additions
			if (Element.Value.TileTags.HasTag(LavaTag))
			{
				//SpawnTransform.SetScale3D(FVector(1.f,1.f,1.f));
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				LavaMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(WaterTag)){
				//SpawnTransform.SetScale3D(FVector(1.f,1.f,1.f));
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				WaterMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(MossTag)){
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				MossMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(IceTag)){
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				IceMesh->AddInstance(SpawnTransform);
			}else if(Element.Value.TileTags.HasTag(RockTag)){
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				RockMesh->AddInstance(SpawnTransform);
			}else if (!LandIndex.Contains(Element.Key))
			{
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
	
				int32 TileIndex = landMesh->AddInstance(SpawnTransform);
				LandIndex.Add(Element.Key, TileIndex);
			}
		}
		else if (Element.Value.TileTags.HasTag(PathTag))
		{
			if (Element.Value.TileTags.HasTag(PathStartTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				pathStartMesh->AddInstance(SpawnTransform);
				continue;
			}
			if (Element.Value.TileTags.HasTag(PathEndTag))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				pathEndMesh->AddInstance(SpawnTransform);
				continue;
			}
			
			if (!PathIndex.Contains(Element.Key))
			{
				SpawnTransform.SetLocation(Element.Value.WorldLocation);
				int32 TileIndex = pathMesh->AddInstance(SpawnTransform);
				PathIndex.Add(Element.Key, TileIndex);
			}
		}
	}
	
}

void AHexGrid::GenerateGrid()
{
	if (bInitialiseGrid)
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
	
	if (bInitialiseGrid && bGeneratePath)
	{
		// generates path then adds path tag to specified tiles
		// create selection method
		//Path = Walker();
		Path = PerlinPaths();
		for (FVector Element : Path)
		{
			if (FTilePropertiesStruct* TileStatus = GridInfo.GridTiles.Find(Element))
			{
				FTilePropertiesStruct NewStatus;
				NewStatus.WorldLocation = TileStatus->WorldLocation;
				//NewStatus.TileStates = PathTag;
				NewStatus.TileTags.Reset();
				NewStatus.TileTags.AddTag(PathTag);
				// marking start and end points of path
				if (Element == Path[0])
				{
					NewStatus.TileTags.AddTag(PathStartTag);
				}else if (Element == (Path[Path.Num()-1])){
					NewStatus.TileTags.AddTag(PathEndTag);
					//EndPoint = Element;
					GridInfo.EndPoint = Element;
				}else
				{
					NewStatus.TileTags.AddTag(PathTag);
				}
				
				
				GridInfo.GridTiles.Add(Element, NewStatus);
			}
		}
	}
	
}

void AHexGrid::GenerateLandscape()
{
	if (bInitialiseGrid && bGenerateLandscape)
	{
		PerlinLandscape();
	}else
	{
		for (auto& tiles : LandIndex)
		{
			//need check for valid transform
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(GridInfo.GridTiles.Find(tiles.Key)->WorldLocation);
			landMesh->UpdateInstanceTransform(tiles.Value, SpawnTransform);
		}
	}
	
}

// Walker algorithm that changes its neighbour acquisition method depending on type selected( DW or PW)
//TArray<FVector> AHexGrid::Walker(Selection name)
TArray<FVector> AHexGrid::Walker()
{
	int _attempts = 0;
	TArray<FVector> _TestPath;
	bool _generate = true;
	
	while (_attempts <= IterationAttempts && _generate)
	{
		FVector _CurrentTile;
		_TestPath.Empty();
		int _CurrentSteps = 0;
		_attempts++;
		
		if (GridInfo.GridTiles.Contains(GridInfo.StartPoint))
		{
			_CurrentTile = GridInfo.StartPoint;
			_TestPath.Add(_CurrentTile);
			_CurrentSteps++;
			TArray<ETileNeighbour> _VisitedTiles;
			bool _pathBlocked = false;
			
			while (_CurrentSteps < PathSize && !_pathBlocked)
			{
				// DW method
				/*
				const int _MaxChoice = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
				ETileNeighbour _ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, _MaxChoice));
				

				// re-selects another neighbour if already visited
				while (_VisitedTiles.Contains(_ChosenNeighbour))
				{
					if (_VisitedTiles.Num() == _MaxChoice)
					{
						// break current iteration
						_pathBlocked = true;
						break;
					}else
					{
						_ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, _MaxChoice));
					}
					
				}

				*/
				
				// DW or PW method
				ETileNeighbour _ChosenNeighbour;
				TPair<bool, ETileNeighbour> NeighbourTile;
				if (SelectDrunkardsWalk)
				{
					NeighbourTile = DrunkardsWalk(_VisitedTiles);
					_ChosenNeighbour = NeighbourTile.Value;
				}else
				{
					NeighbourTile = PerlinWorm(_CurrentTile, _VisitedTiles);
					_ChosenNeighbour = NeighbourTile.Value;
				}
				
				if (NeighbourTile.Key)
				{
					_pathBlocked = true;
					break;
				}
				
				//testing
				//_ChosenNeighbour = static_cast<ETileNeighbour>(0);
				
				TPair<FVector, bool> _Neighbour;

				switch (_ChosenNeighbour)
				{
				case ETileNeighbour::North:
					_Neighbour = NorthNeighbour(_CurrentTile);
					if (_Neighbour.Value && !_TestPath.Contains(_Neighbour.Key))
					{
						_TestPath.Add(_Neighbour.Key);
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
					_Neighbour = NorthEastNeighbour(_CurrentTile);
					if (_Neighbour.Value && !_TestPath.Contains(_Neighbour.Key))
					{
						_TestPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Northeast);
					}
					break;
				case ETileNeighbour::Southeast:
					_Neighbour = SouthEastNeighbour(_CurrentTile);
					if (_Neighbour.Value && !_TestPath.Contains(_Neighbour.Key))
					{
						_TestPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Southeast);
					}
					break;
				case ETileNeighbour::South:
					_Neighbour = SouthNeighbour(_CurrentTile);
					if (_Neighbour.Value && !_TestPath.Contains(_Neighbour.Key))
					{
						_TestPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::South);
					}
					break;
				case ETileNeighbour::Southwest:
					_Neighbour = SouthWestNeighbour(_CurrentTile);
					if (_Neighbour.Value && !_TestPath.Contains(_Neighbour.Key))
					{
						_TestPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Southwest);
					}
					break;
				case ETileNeighbour::Northwest:
					_Neighbour = NorthWestNeighbour(_CurrentTile);
					if (_Neighbour.Value && !_TestPath.Contains(_Neighbour.Key))
					{
						_TestPath.Add(_Neighbour.Key);
						_CurrentTile = _Neighbour.Key;
						_CurrentSteps++;
						_VisitedTiles.Empty();
					}else
					{
						_VisitedTiles.Add(ETileNeighbour::Northwest);
					}
					break;
				}

				if (_TestPath.Num() >= PathSize)
				{
					// stop loops, successful path generation
					_generate = false;
					break;
				}
			}
		}
	}
	
	CurrentIteration = _attempts;
	return _TestPath;
}

void AHexGrid::PerlinLandscape()
{
	// need to find method & parameters to adjust noise value

	for (auto& Tile : GridInfo.GridTiles)
	{
	
		if (Tile.Value.TileTags.HasTag(LandTag))
		{
			const float Frequency =  1.0f / FeatureScale;
			// float NoiseValue = FMath::PerlinNoise2D(FVector2D(Tile.Key.X * NoiseScale + 0.1, Tile.Key.Y * NoiseScale + 0.1))
			float NoiseValue = FMath::PerlinNoise2D(FVector2D(Tile.Key.X , Tile.Key.Y ) * Frequency + NoiseOffset );
			
			// normalising, since above function returns a value from -1 to 1
			NoiseValue = (NoiseValue + 1) / 2;

			// adding to scale
			NoiseValue = NoiseValue * HeightMultiplier;
			Tile.Value.TileHeight = NoiseValue;
			
		}
	}
}

TArray<FVector> AHexGrid::PerlinPaths()
{
	TArray<FPerlinWorm> Worms;

	// configuration parameters (for each individual worm)(expose and make struct)
	int Length = 10;
	int Seed = 12345;
	float Freq = 0.1;
	float TurnSpeed = 1;

	FPerlinWorm TestWorm(FVector2D(GridInfo.StartPoint.X, GridInfo.StartPoint.Y), PathTag);
	//FVector2D CurrentPos = FVector2D(GridInfo.StartPoint.X, GridInfo.StartPoint.Y);
	float Angle = 0.0f;

	for (int i = 0; i < Length; i++)
	{
		float Noise = FMath::PerlinNoise2D(FVector2D((TestWorm.GetX() + Seed) * Freq, (TestWorm.GetY() + Seed) * Freq));
		// normalising (0 to 1) then applying it to degrees
		Angle = ((Noise + 1) / 2) * 360;
		
		ETileNeighbour Dir = UTileDirectionUtils::GetDirectionFromAngle(Angle);
		FVector NeighbourPos = UTileDirectionUtils::GetNeighbourPos(Dir, FVector(TestWorm.GetX(), TestWorm.GetY(), 0));
		FVector2D StepPos = FVector2D(NeighbourPos.X, NeighbourPos.Y);
		
		TestWorm.Grow(StepPos);
	}
	Worms.Add(TestWorm);
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

void AHexGrid::VoronoiRegions()
{
	if (!bGenerateRegions) return;
	
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
		NewCoord.X = FMath::RandRange(0,Columns);
		NewCoord.Y = FMath::RandRange(0,Rows);
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
	
	for (int y = 0; y < Rows; y++)
	{
		for (int x = 0; x < Columns; x++)
		{
			if (x % 2 == 0)
			{
				// even hex tiles & their respective calculations
				FVector GridCoord = FVector(x, y, 0.0f);
				FVector TileLocation;
				TileLocation.X = _CalculateTileHeight() * y;
				TileLocation.Y = (TileRadius * 2) * 0.75 * x;
				TileLocation.Z = 0;

				FTilePropertiesStruct Tile;
				Tile.WorldLocation = TileLocation;
				//Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				//Tile.TileStates = LandTag;
				Tile.TileTags.AddTag(LandTag);

				GridInfo.GridTiles.Add(GridCoord, Tile);
			}
			else
			{
				// odd hex tiles & their respective calculations
				FVector GridCoord = FVector(x, y, 0.0f);
				FVector TileLocation;
				TileLocation.X = (_CalculateTileHeight() * y) + (_CalculateTileHeight() / 2);
				TileLocation.Y = (TileRadius * 2) * 0.75 * x;
				TileLocation.Z = 0;

				FTilePropertiesStruct Tile;
				Tile.WorldLocation = TileLocation;
				//Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				//Tile.TileStates = LandTag;
				Tile.TileTags.AddTag(LandTag);

				GridInfo.GridTiles.Add(GridCoord, Tile);
			}
		}
	}

	//if (GridInfo.Contains(StartPoint)) GridInfo[StartPoint].TileStates = PathStartTag;
	if (GridInfo.GridTiles.Contains(GridInfo.StartPoint))
	{
		GridInfo.GridTiles[GridInfo.StartPoint].TileTags.AddLeafTag(PathStartTag);
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

// calculations assume Even-Q hex grid
TPair<FVector, bool> AHexGrid::NorthNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour(UTileDirectionUtils::NorthNeighbourCoords(CurrentTile));
	/*
	TileNeighbour.X = CurrentTile.X;
	TileNeighbour.Y = CurrentTile.Y + 1;
	TileNeighbour.Z = CurrentTile.Z;
	*/

	if (GridInfo.GridTiles.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::NorthEastNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour(UTileDirectionUtils::NorthEastNeighbourCoords(CurrentTile));
	/*
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		TileNeighbour.X = CurrentTile.X + 1;
		TileNeighbour.Y = CurrentTile.Y;
		TileNeighbour.Z = CurrentTile.Z;
		
	}else
	{
		TileNeighbour.X = CurrentTile.X + 1;
		TileNeighbour.Y = CurrentTile.Y + 1;
		TileNeighbour.Z = CurrentTile.Z;
	}
	*/

	if (GridInfo.GridTiles.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
	
}

TPair<FVector, bool> AHexGrid::SouthEastNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour(UTileDirectionUtils::SouthEastNeighbourCoords(CurrentTile));
	/*
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		TileNeighbour.X = CurrentTile.X + 1;
		TileNeighbour.Y = CurrentTile.Y - 1;
		TileNeighbour.Z = CurrentTile.Z;
		
	}else
	{
		TileNeighbour.X = CurrentTile.X + 1;
		TileNeighbour.Y = CurrentTile.Y;
		TileNeighbour.Z = CurrentTile.Z;
	}
	*/
	if (GridInfo.GridTiles.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::SouthNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour(UTileDirectionUtils::SouthNeighbourCoords(CurrentTile));
	/*
	TileNeighbour.X = CurrentTile.X;
	TileNeighbour.Y = CurrentTile.Y - 1;
	TileNeighbour.Z = CurrentTile.Z;
	*/
	if (GridInfo.GridTiles.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::SouthWestNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour(UTileDirectionUtils::SouthWestNeighbourCoords(CurrentTile));
	/*
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		TileNeighbour.X = CurrentTile.X - 1;
		TileNeighbour.Y = CurrentTile.Y - 1;
		TileNeighbour.Z = CurrentTile.Z;
		
	}else
	{
		TileNeighbour.X = CurrentTile.X - 1;
		TileNeighbour.Y = CurrentTile.Y;
		TileNeighbour.Z = CurrentTile.Z;
	}
	*/
	if (GridInfo.GridTiles.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::NorthWestNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour(UTileDirectionUtils::NorthWestNeighbourCoords(CurrentTile));
	/*
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		TileNeighbour.X = CurrentTile.X - 1;
		TileNeighbour.Y = CurrentTile.Y;
		TileNeighbour.Z = CurrentTile.Z;
		
	}else
	{
		TileNeighbour.X = CurrentTile.X - 1;
		TileNeighbour.Y = CurrentTile.Y + 1;
		TileNeighbour.Z = CurrentTile.Z;
	}
	*/
	if (GridInfo.GridTiles.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

bool AHexGrid::TileOnBoundary(const FVector& CurrentTile) const
{
	if (CurrentTile.X == 0 || CurrentTile.X == Columns - 1) return true;
	if (CurrentTile.Y == 0 || CurrentTile.Y == Rows - 1) return true;

	return false;
	
}

FGameplayTag AHexGrid::GetRegionTag(const ERegionType Type) const
{
	switch (Type)
	{
		case ERegionType::Lava:
			return LavaTag;
		case ERegionType::Water:
			return WaterTag;
		case ERegionType::Moss:
			return MossTag;
		case ERegionType::Ice:
			return IceTag;
		case ERegionType::Rock:
			return RockTag;
		default:
			return FGameplayTag::EmptyTag;
	}
}

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


float AHexGrid::_CalculateTileHeight() const
{
	return (TileRadius * sqrt(3));
}

void AHexGrid::_clearPath()
{
	pathMesh->ClearInstances();
	PathIndex.Empty();
}

void AHexGrid::_clearLand()
{
	pathStartMesh->ClearInstances();
	pathEndMesh->ClearInstances();
	landMesh->ClearInstances();
	LandIndex.Empty();
}

void AHexGrid::_clearRegions()
{
	LavaMesh->ClearInstances();
	WaterMesh->ClearInstances();
	MossMesh->ClearInstances();
	IceMesh->ClearInstances();
	RockMesh->ClearInstances();
}

void AHexGrid::_ClearGrid()
{
	_clearPath();
	_clearLand();
	_clearRegions();
}
