// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AHexGrid::AHexGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
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
		}else
		{
			CalculateGrid();
			ConstructGrid();
		}
	}else
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
			FTransform spawnTransform;
			spawnTransform.SetLocation(element.Value.RelativeLocation);
			if (landMesh) landMesh->AddInstance(spawnTransform);
		}
		if (TileTag == UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Path"))
		{
			FTransform spawnTransform;
			spawnTransform.SetLocation(element.Value.RelativeLocation);
			if (pathMesh) pathMesh->AddInstance(spawnTransform);
		}
	}
}

void AHexGrid::DrunkardsWalk()
{
	// local variables to path calculation
	FVector CurrentTile;
	TArray<FVector> TestPath;
	int CurrentSteps = 0;
	int CurrentIteration = 0;
	
	if (GridInfo.Find(StartPoint))
	{
		CurrentTile = StartPoint;
		TestPath.Add(CurrentTile);
		CurrentSteps++;
		
		while(CurrentSteps < PathSize && CurrentIteration < IterationAttempts)
		{
			const int MaxChoice = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
			const int INTChoice = FMath::RandRange(0, MaxChoice);
			TPair<FVector, bool> Neighbour;

			switch (uint8_t EnumChoice =  static_cast<uint8_t>(INTChoice); static_cast<ETileNeighbour>(EnumChoice))
			{
			case ETileNeighbour::North:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !TestPath.Contains(Neighbour.Key))
				{
					TestPath.Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			case ETileNeighbour::Northeast:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !TestPath.Contains(Neighbour.Key))
				{
					TestPath.Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			case ETileNeighbour::Southeast:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !TestPath.Contains(Neighbour.Key))
				{
					TestPath.Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			case ETileNeighbour::South:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !TestPath.Contains(Neighbour.Key))
				{
					TestPath.Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			case ETileNeighbour::Southwest:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !TestPath.Contains(Neighbour.Key))
				{
					TestPath.Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			case ETileNeighbour::Northwest:
				Neighbour = NorthNeighbour(CurrentTile);
				if (Neighbour.Value && !TestPath.Contains(Neighbour.Key))
				{
					TestPath.Add(Neighbour.Key);
					CurrentSteps++;
					CurrentTile = Neighbour.Key;
				}else
				{
					CurrentSteps = PathSize;
				}
				break;
			
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
				FVector GridCoord = FVector(x,y,0.0f);
				FVector TileLocation;
				TileLocation.X = _CalculateTileHeight() * y;
				TileLocation.Y = (TileRadius * 2) * 0.75 * x;
				TileLocation.Z = 0;
				
				FTilePropertiesStruct Tile;
				Tile.RelativeLocation = TileLocation;
				Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				
				GridInfo.Add(GridCoord,Tile);
			}else
			{
				// odd hex tiles & their respective calculations
				FVector GridCoord = FVector(x,y,0.0f);
				FVector TileLocation;
				TileLocation.X = (_CalculateTileHeight() * y) + (_CalculateTileHeight() / 2);
				TileLocation.Y = (TileRadius * 2) * 0.75 * x;
				TileLocation.Z = 0;
				
				FTilePropertiesStruct Tile;
				Tile.RelativeLocation = TileLocation;
				Tile.TileStates = UGameplayTagsManager::Get().RequestGameplayTag("MapGeneration.Initialised");
				
				GridInfo.Add(GridCoord,Tile);
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

void AHexGrid::NorthEastNeighbour()
{
}

void AHexGrid::SouthEastNeighbour()
{
}

void AHexGrid::SouthNeighbour()
{
}

void AHexGrid::SouthWestNeighbour()
{
}

void AHexGrid::NorthWestNeighbour()
{
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

