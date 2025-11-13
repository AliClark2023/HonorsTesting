// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"

#include "MathUtil.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/TransformCalculus3D.h"
#include "NavMesh/RecastNavMesh.h"
#include "Runtime/Datasmith/CADKernel/Base/Public/UI/Visu.h"

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

	// clean up
	/*
	if (bInitialiseGrid)
	{
		if (bGenerate)
		{
			CalculateGrid();
			DrunkardsWalk();
			if (bAddHeight) PerlinLandscape();
			ConstructGrid();
		}
		else
		{
			CalculateGrid();
			//testing perlin only
			//if (bAddHeight) PerlinLandscape();
			
			if (GridInfo.Contains(StartPoint)) GridInfo[StartPoint].TileStates = PathStartTag;
			ConstructGrid();
		}
	}
	else
	{
		//GridInfo.Empty();
		_ClearGrid();
	}
	*/

	// new method
	
	GenerateGrid();
	GeneratePath();
	GenerateLandscape();
	
	// generate regions
	
}

void AHexGrid::ConstructGrid()
{
	/* // old method
	for (auto& Element : GridInfo)
	{
		FGameplayTag TileTag = Element.Value.TileStates;

		if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised"))
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Element.Value.WorldLocation);
			landMesh->AddInstance(SpawnTransform);
		}
		if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path"))
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Element.Value.WorldLocation);
			pathMesh->AddInstance(SpawnTransform);
		}
	}
	*/

	// step method
	for (auto& Element : GridInfo)
	{
		FTransform SpawnTransform;
		
		switch (Element.Value.TileStates)
		{
		case LandTag:
			SpawnTransform.SetLocation(Element.Value.WorldLocation);

			if (!GridToLandInstanceIndex.Contains(Element.Key))
			{
				int32 TileIndex = landMesh->AddInstance(SpawnTransform);
				GridToLandInstanceIndex.Add(Element.Key, TileIndex);
			}
			break;
		case PathStartTag:
			SpawnTransform.SetLocation(Element.Value.WorldLocation);
			pathStartMesh->AddInstance(SpawnTransform);
			break;
		default:
			// do nothing, will help identify if tags aren't being properly assigned
			break;
		}
	}
	/*
	for (auto& Element : GridInfo)
	{
		FGameplayTag TileTag = Element.Value.TileStates;
		
		//if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised"))
		if (TileTag == LandTag)
		{
			FTransform SpawnTransform;
			if (bAddHeight)
			{
				SpawnTransform.SetScale3D(FVector(1.f,1.f,Element.Value.TileHeight));
			}else
			{
				SpawnTransform.SetScale3D(FVector(1.f,1.f,1.f));
			}
			SpawnTransform.SetLocation(Element.Value.WorldLocation);
			
			// Checks and replaces path tile with land tile
			if (GridToPathInstanceIndex.Contains(Element.Key))
			{
				//removal from path
				int32* PathIndex = GridToPathInstanceIndex.Find(Element.Key);
				if (PathIndex)
				{
					pathMesh->RemoveInstance(*PathIndex);
					GridToPathInstanceIndex.Remove(Element.Key);
					
					//updating index values (need to decrease all index values above found index)
					for (auto& Index : GridToPathInstanceIndex)
					{
						if (Index.Value > *PathIndex)
						{
							Index.Value = Index.Value - 1;
						}
					}
					

					//add to land instances
					int32 landIndex = landMesh->AddInstance(SpawnTransform);
					GridToLandInstanceIndex.Add(Element.Key, landIndex);
					continue;
				}
				
			}
			// adds land tile to container and keeps track of its index location
			if (!GridToLandInstanceIndex.Contains(Element.Key))
			{
				int32 TileIndex = landMesh->AddInstance(SpawnTransform);
				GridToLandInstanceIndex.Add(Element.Key, TileIndex);
			}else
			{
				continue;
			}
		}
		if (TileTag == PathStartTag)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Element.Value.WorldLocation);
			pathStartMesh->AddInstance(SpawnTransform);
		}
		//if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path"))
		if (TileTag == PathTag)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Element.Value.WorldLocation);
	

			// Checks and replaces land tile with path tile
			if (GridToLandInstanceIndex.Contains(Element.Key))
			{
				//removal from Land
				int32* LandIndex = GridToLandInstanceIndex.Find(Element.Key);
				if (LandIndex)
				{
					landMesh->RemoveInstance(*LandIndex);
					GridToLandInstanceIndex.Remove(Element.Key);
					
					//updating index values
					for (auto& Index : GridToLandInstanceIndex)
					{
						if (Index.Value > *LandIndex)
						{
							Index.Value = Index.Value - 1;
						}
					}

					//add to path instances
					int32 pathIndex = pathMesh->AddInstance(SpawnTransform);
					GridToPathInstanceIndex.Add(Element.Key, pathIndex);
					continue;
				}
				
			}
			// adds path tile to container and keeps track of its index location
			int32 TileIndex= pathMesh->AddInstance(SpawnTransform);
			GridToPathInstanceIndex.Add(Element.Key, TileIndex);
		}
	}
	*/
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
	if (bGeneratePath)
	{
		// generates path then replaces tiles with path tiles
		TArray<FVector> Path = DrunkardsWalk();
		
		for (FVector Element : Path)
		{
			if (FTilePropertiesStruct* TileStatus = GridInfo.Find(Element))
			{
				FTilePropertiesStruct NewStatus;
				NewStatus.WorldLocation = TileStatus->WorldLocation;
				NewStatus.TileStates = PathTag;

				GridInfo.Add(Element, NewStatus);

				//replacing starttile with path tile
				if (Element == StartPoint)
				{
					pathStartMesh->ClearInstances();

					//add to path instances
					FTransform SpawnTransform;
					SpawnTransform.SetLocation(TileStatus->WorldLocation);
					int32 PathIndex = pathMesh->AddInstance(SpawnTransform);
					GridToPathInstanceIndex.Add(Element, PathIndex);
				}
				//replacing landtile with path tile
				if (int32* LandIndex = GridToLandInstanceIndex.Find(Element))
				{
					FTransform SpawnTransform;
					SpawnTransform.SetLocation(TileStatus->WorldLocation);
					//removal from Land
					if (LandIndex)
					{
						landMesh->RemoveInstance(*LandIndex);
						GridToLandInstanceIndex.Remove(Element);
					
						//updating index values
						for (auto& Index : GridToLandInstanceIndex)
						{
							if (Index.Value > *LandIndex)
							{
								Index.Value = Index.Value - 1;
							}
						}

						//add to path instances
						int32 pathIndex = pathMesh->AddInstance(SpawnTransform);
						GridToPathInstanceIndex.Add(Element, pathIndex);
						continue;
					}
				}
			}
		}
	} // replace path tiles to default tiles
	else
	{
		for (auto& Element : GridInfo)
		{
			// Checks and replaces path tile with land tile
			if (GridToPathInstanceIndex.Contains(Element.Key))
			{
				FTransform SpawnTransform;
				//removal from path
				int32* PathIndex = GridToPathInstanceIndex.Find(Element.Key);
				if (PathIndex)
				{
					pathMesh->RemoveInstance(*PathIndex);
					GridToPathInstanceIndex.Remove(Element.Key);
					
					//updating index values (need to decrease all index values above found index)
					for (auto& Index : GridToPathInstanceIndex)
					{
						if (Index.Value > *PathIndex)
						{
							Index.Value = Index.Value - 1;
						}
					}
					

					//add to land instances
					int32 landIndex = landMesh->AddInstance(SpawnTransform);
					GridToLandInstanceIndex.Add(Element.Key, landIndex);
					continue;
				}
			}
		}
	}
}

void AHexGrid::GenerateLandscape()
{
}

TArray<FVector> AHexGrid::DrunkardsWalk()
{
	int _attempts = 0;
	TArray<FVector> _TestPath;
	bool _generate = true;
	
	while (_attempts <= IterationAttempts && _generate)
	{
		FVector _CurrentTile;
		//TArray<FVector> _TestPath;
		_TestPath.Empty();
		int _CurrentSteps = 0;
		_attempts++;
		
		if (GridInfo.Contains(StartPoint))
		{
			_CurrentTile = StartPoint;
			_TestPath.Add(_CurrentTile);
			_CurrentSteps++;
			TArray<ETileNeighbour> _VisitedTiles;
			bool _pathBlocked = false;
			
			while (_CurrentSteps < PathSize && !_pathBlocked)
			{
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
						// remove when visited tiles is implemented
						//_CurrentSteps++;
						
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

	// Applying full or partial path to grid
	/*
	for (FVector Element : _TestPath)
	{
		if (FTilePropertiesStruct* TileStatus = GridInfo.Find(Element))
		{
			FTilePropertiesStruct NewStatus;
			NewStatus.WorldLocation = TileStatus->WorldLocation;
			//NewStatus.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path");
			NewStatus.TileStates = PathTag;

			GridInfo.Add(Element, NewStatus);
		}
	}
	*/
	CurrentIteration = _attempts;
	return _TestPath;
}

void AHexGrid::PerlinLandscape()
{
	// need to find method & parameters to adjust noise value

	for (auto& Tile : GridInfo)
	{
		//if (Tile.Value.TileStates == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised"))
		if (Tile.Value.TileStates == LandTag)
		{
			float NoiseValue = FMath::PerlinNoise2D(FVector2D(Tile.Key.X * NoiseScale + 0.1, Tile.Key.Y * NoiseScale + 0.1));
			// normalising, since above function returns a value from -1 to 1
			NoiseValue = (NoiseValue + 1) / 2;

			// adding to scale
			NoiseValue = NoiseValue * HeightMultiplier;
			Tile.Value.TileHeight = NoiseValue;
		}
	}
}

void AHexGrid::CalculateGrid()
{
	//_ClearGrid();
	if (!GridInfo.IsEmpty()) GridInfo.Empty();
	
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
				Tile.TileStates = LandTag;

				GridInfo.Add(GridCoord, Tile);
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
				Tile.TileStates = LandTag;

				GridInfo.Add(GridCoord, Tile);
			}
		}
	}
}

// calculations assume Even-Q hex grid
TPair<FVector, bool> AHexGrid::NorthNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	TileNeighbour.X = CurrentTile.X;
	TileNeighbour.Y = CurrentTile.Y + 1;
	TileNeighbour.Z = CurrentTile.Z;

	if (GridInfo.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::NorthEastNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	
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

	if (GridInfo.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
	
}

TPair<FVector, bool> AHexGrid::SouthEastNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	
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

	if (GridInfo.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::SouthNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	TileNeighbour.X = CurrentTile.X;
	TileNeighbour.Y = CurrentTile.Y - 1;
	TileNeighbour.Z = CurrentTile.Z;

	if (GridInfo.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::SouthWestNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	
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

	if (GridInfo.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

TPair<FVector, bool> AHexGrid::NorthWestNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	
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

	if (GridInfo.Contains(TileNeighbour) && !TileOnBoundary(TileNeighbour))
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


float AHexGrid::_CalculateTileHeight() const
{
	return (TileRadius * sqrt(3));
}

void AHexGrid::_ClearGrid()
{
	/*
	if (!GridInfo.IsEmpty())
	{
		GridInfo.Empty();
		if (pathMesh)
		{
			pathMesh->ClearInstances();
			GridToPathInstanceIndex.Empty();
		}
		if (landMesh)
		{
			landMesh->ClearInstances();
			GridToLandInstanceIndex.Empty();
		}
	}
	*/

	pathMesh->ClearInstances();
	landMesh->ClearInstances();
	pathStartMesh->ClearInstances();
	GridToPathInstanceIndex.Empty();
	GridToLandInstanceIndex.Empty();
}
