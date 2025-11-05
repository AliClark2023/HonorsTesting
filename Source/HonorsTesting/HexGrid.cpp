// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"

#include "Components/InstancedStaticMeshComponent.h"

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
	// local variables to path calculation
	FVector CurrentTile;
	TUniquePtr<TArray<FVector>> _TestPath = MakeUnique<TArray<FVector>>();
	//_TestPath->Empty();
	int CurrentSteps = 0;

	
		

	// resetting iteration count
	if (CurrentIteration == IterationAttempts)
	{
		CurrentIteration = 0;
	}
	

	if (GridInfo.Contains(StartPoint))
	{
		
		CurrentTile = StartPoint;
		_TestPath->Emplace(CurrentTile);
		CurrentSteps++;

		while (CurrentSteps < PathSize && CurrentIteration < IterationAttempts)
		{
			const int MaxChoice = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
			//int INTChoice = FMath::RandRange(0, MaxChoice);

			//testing
			//INTChoice = 5;
			ETileNeighbour ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, MaxChoice));
			TPair<FVector, bool> Neighbour;
			
			switch (ChosenNeighbour)
			{
			case ETileNeighbour::North:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !_TestPath->Contains(Neighbour.Key))
				{
					_TestPath->Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps++;
				}
				break;
			case ETileNeighbour::Northeast:
				Neighbour = NorthEastNeighbour(CurrentTile);
				if (Neighbour.Value && !_TestPath->Contains(Neighbour.Key))
				{
					_TestPath->Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps++;
				}
				break;
			case ETileNeighbour::Southeast:
				Neighbour = SouthEastNeighbour(CurrentTile);
				if (Neighbour.Value && !_TestPath->Contains(Neighbour.Key))
				{
					_TestPath->Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps++;
				}
				break;
			case ETileNeighbour::South:
				Neighbour = SouthNeighbour(CurrentTile);
				if (Neighbour.Value && !_TestPath->Contains(Neighbour.Key))
				{
					_TestPath->Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps++;
				}
				break;
			case ETileNeighbour::Southwest:
				Neighbour = SouthWestNeighbour(CurrentTile);
				if (Neighbour.Value && !_TestPath->Contains(Neighbour.Key))
				{
					_TestPath->Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			case ETileNeighbour::Northwest:
				Neighbour = NorthWestNeighbour(CurrentTile);
				if (Neighbour.Value && !_TestPath->Contains(Neighbour.Key))
				{
					_TestPath->Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps++;
				}
				break;
			}
		}

		if (_TestPath->Num() < PathSize && CurrentIteration < IterationAttempts)
		{
			CurrentIteration++;
			//TestPath.Empty();
			DrunkardsWalk();
		}
		else
		{
			// Generate full or partial path
			for (FVector& Element : *_TestPath)
			{
				if (FTilePropertiesStruct* TileStatus = GridInfo.Find(Element))
				{
					FTilePropertiesStruct NewStatus;
					NewStatus.WorldLocation = TileStatus->WorldLocation;
					NewStatus.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path");

					GridInfo.Add(Element, NewStatus);
				}
			}
		}
	}
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
