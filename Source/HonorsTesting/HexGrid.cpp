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
			//DrunkardsWalk();
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

void AHexGrid::NorthNeighbour()
{
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

