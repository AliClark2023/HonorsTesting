// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "NavMesh/RecastNavMesh.h"

// Sets default values
AHexGrid::AHexGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Attaching components
	landMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LandMesh"));
	RootComponent = landMesh;
	pathMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PathMesh"));
	pathMesh->SetupAttachment(RootComponent);
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

	if (bInitialiseGrid)
	{
		if (bGenerate)
		{
			CalculateGrid();
			DrunkardsWalk();
			ConstructGrid();
		}
		else
		{
			CalculateGrid();
			ConstructGrid();
		}
	}
	else
	{
		_ClearGrid();
	}
}

void AHexGrid::ConstructGrid()
{
	for (auto& element : GridInfo)
	{
		FGameplayTag TileTag = element.Value.TileStates;

		if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised"))
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(element.Value.WorldLocation);
			if (landMesh) landMesh->AddInstance(SpawnTransform);
		}
		if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path"))
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(element.Value.WorldLocation);
			if (pathMesh) pathMesh->AddInstance(SpawnTransform);
		}
	}
}

void AHexGrid::DrunkardsWalk()
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
	for (FVector Element : _TestPath)
	{
		if (FTilePropertiesStruct* TileStatus = GridInfo.Find(Element))
		{
			FTilePropertiesStruct NewStatus;
			NewStatus.WorldLocation = TileStatus->WorldLocation;
			NewStatus.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path");

			GridInfo.Add(Element, NewStatus);
		}
	}
	CurrentIteration = _attempts;
}

void AHexGrid::CalculateGrid()
{
	_ClearGrid();
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
				Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");

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
				Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				//Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path");
				

				GridInfo.Add(GridCoord, Tile);
			}
		}
	}
}

TPair<FVector, bool> AHexGrid::NorthNeighbour(const FVector& CurrentTile) const
{
	FVector TileNeighbour;
	TileNeighbour.X = CurrentTile.X;
	TileNeighbour.Y = CurrentTile.Y + 1;
	TileNeighbour.Z = CurrentTile.Z;

	if (GridInfo.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}

// assume Even-Q hex grid
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

	if (GridInfo.Contains(TileNeighbour))
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

	if (GridInfo.Contains(TileNeighbour))
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

	if (GridInfo.Contains(TileNeighbour))
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

	if (GridInfo.Contains(TileNeighbour))
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

	if (GridInfo.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(TileNeighbour, true);
	}
	return TPair<FVector, bool>(TileNeighbour, false);
}


float AHexGrid::_CalculateTileHeight() const
{
	return (TileRadius * sqrt(3));
}

void AHexGrid::_ClearGrid()
{
	if (!GridInfo.IsEmpty())
	{
		GridInfo.Empty();
		if (pathMesh) pathMesh->ClearInstances();
		if (landMesh) landMesh->ClearInstances();
	}
}
