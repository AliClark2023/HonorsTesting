#include "TileDirectionUtils.h"

#include "Runtime/Core/Internal/IO/PlatformIoDispatcher.h"

// angle must be between 0 and 360
ETileNeighbour UTileDirectionUtils::GetDirectionFromAngle(float Angle)
{
	if (Angle < 60.f)   return ETileNeighbour::North;
	if (Angle < 120.f)  return ETileNeighbour::Northeast;
	if (Angle < 180.f)  return ETileNeighbour::Southeast;
	if (Angle < 240.f)  return ETileNeighbour::South;
	if (Angle < 300.f)  return ETileNeighbour::Southwest;
	return ETileNeighbour::Northwest;
}
// returns position of opposite neighbour, used for responding to boundaries
FVector UTileDirectionUtils::GetOppositeNeighbour(ETileNeighbour CurrentNeighbour, const FVector& CurrentTile)
{
	switch (CurrentNeighbour)
	{
	case ETileNeighbour::North:
		return  UTileDirectionUtils::SouthNeighbourCoords(CurrentTile);
	case ETileNeighbour::Northeast:
		return  UTileDirectionUtils::SouthWestNeighbourCoords(CurrentTile);
	case ETileNeighbour::Southeast:
		return  UTileDirectionUtils::NorthWestNeighbourCoords(CurrentTile);
	case ETileNeighbour::South:
		return  UTileDirectionUtils::NorthNeighbourCoords(CurrentTile);
	case ETileNeighbour::Southwest:
		return  UTileDirectionUtils::NorthEastNeighbourCoords(CurrentTile);
	case ETileNeighbour::Northwest:
		return  UTileDirectionUtils::SouthEastNeighbourCoords(CurrentTile);
	default:
		return CurrentTile;
	}
}

/*
 * Following functions assume an even-Q grid formation
 * Adjust these if changing tile formation.
 */

FVector UTileDirectionUtils::NorthNeighbourCoords(const FVector& CurrentTile)
{
	return FVector(CurrentTile.X, CurrentTile.Y + 1, CurrentTile.Z);
}

FVector UTileDirectionUtils::NorthEastNeighbourCoords(const FVector& CurrentTile)
{
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		return FVector(CurrentTile.X + 1, CurrentTile.Y, CurrentTile.Z);
	}
	
	return FVector(CurrentTile.X + 1, CurrentTile.Y + 1, CurrentTile.Z);
}

FVector UTileDirectionUtils::SouthEastNeighbourCoords(const FVector& CurrentTile)
{
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		return FVector(CurrentTile.X + 1, CurrentTile.Y - 1, CurrentTile.Z);
	}
	return FVector(CurrentTile.X + 1, CurrentTile.Y, CurrentTile.Z);
}

FVector UTileDirectionUtils::SouthNeighbourCoords(const FVector& CurrentTile)
{
	return FVector(CurrentTile.X, CurrentTile.Y - 1, CurrentTile.Z);
}

FVector UTileDirectionUtils::SouthWestNeighbourCoords(const FVector& CurrentTile)
{
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		return FVector(CurrentTile.X - 1, CurrentTile.Y - 1, CurrentTile.Z);
	}
	return FVector(CurrentTile.X - 1, CurrentTile.Y, CurrentTile.Z);
}

FVector UTileDirectionUtils::NorthWestNeighbourCoords(const FVector& CurrentTile)
{
	if (static_cast<int>(CurrentTile.X) % 2 == 0)
	{
		return FVector(CurrentTile.X - 1, CurrentTile.Y, CurrentTile.Z);
	}
	return FVector(CurrentTile.X - 1, CurrentTile.Y + 1, CurrentTile.Z);
}

// checks if tile is within the boundary - 1
bool UTileDirectionUtils::IsTileBeforeBoundary(const int GridColumn, const int GridRow, const FVector& CurrentTile)
{
	// old (broken?) method
	/*
	if (CurrentTile.X <= 1 || CurrentTile.X >= GridColumn - 2) return true;
	if (CurrentTile.Y <= 1 || CurrentTile.Y >= GridRow - 2) return true;
	return false;
	*/

	// new method
	bool InBounds = CurrentTile.X >=1 && CurrentTile.X < GridColumn - 1 &&
		CurrentTile.Y >= 1 && CurrentTile.Y < GridRow - 1;
	return InBounds;

}
// checks if tile on the boundary
bool UTileDirectionUtils::IsTileOnBoundary(const int GridColumn, const int GridRow, const FVector& CurrentTile)
{
	if (CurrentTile.X <= 1 || CurrentTile.X >= GridColumn - 1) return true;
	if (CurrentTile.Y <= 1 || CurrentTile.Y >= GridRow - 1) return true;

	return false;
}
//sourced from: https://www.redblobgames.com/grids/hexagons/#conversions
// converts Even Q coords to Cube coordinates 
FIntVector UTileDirectionUtils::EvenQToCube(const FVector& EvenQ)
{
	const int EQx = EvenQ.X;
	const int EQy = EvenQ.Y;

	const int x = EQx;
	const int z = EQy - (EQx + (EQx & 1)) / 2;
	const int y = -x - z;

	return FIntVector(x,y,z);
}

FVector UTileDirectionUtils::CubeToEvenQ(const FIntVector& Cube)
{
	const int X = Cube.X;
	const int Y = Cube.Z + (X + (X & 1)) / 2;
	return FVector(X,Y,0.f);
}

// manhattan distance in cube space
int UTileDirectionUtils::CubeDistance(const FIntVector& A, const FIntVector& B)
{
	return (FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y) + FMath::Abs(A.Z - B.Z)) / 2;
}
//converting int cube coords to float for interpolation
FVector UTileDirectionUtils::CubeToFloat(const FIntVector& C)
{
	return FVector(static_cast<float>(C.X), static_cast<float>(C.Y),static_cast<float>(C.Y));
}

//rounding float cube back to int cube
FIntVector UTileDirectionUtils::CubeRound(const FVector& C)
{
	int rx = FMath::RoundToInt(C.X);
	int ry = FMath::RoundToInt(C.Y);
	int rz = FMath::RoundToInt(C.Z);

	float dx = FMath::Abs(rx - C.X);
	float dy = FMath::Abs(rx - C.Y);
	float dz = FMath::Abs(rx - C.Z);

	if (dx > dy && dx > dz)
	{
		rx = -ry -rz;
	}else if (dy > dz)
	{
		ry = -rx - rz;
	}else
	{
		rz = -rx - ry;
	}

	return FIntVector(rx, ry, rz);
}


// determining if grid contains islands

int UTileDirectionUtils::CountIslands(TMap<FVector, FTilePropertiesStruct>& GridTiles, FVector2D GridSize, FGameplayTag TagToFind)
{
	int n = GridSize.X;
	int m = GridSize.Y;
	int Islands = 0;
	TArray<FVector> IslandCentroids;
	
	// Matrix to track visited cells
	TArray<TArray<bool>> Visited;
	Visited.SetNum(n);
	for (int i = 0; i < n; i++)
	{
		Visited[i].SetNumZeroed(m);
	}
	
	for (auto& Tile : GridTiles)
	{
		if (Tile.Value.TileTags.HasTag(TagToFind) && !Visited[Tile.Key.X][Tile.Key.Y])
		{
			IslandCentroids.Add(BFS(GridTiles, Tile.Key, Visited, TagToFind, GridSize));
			// Connected island
			Islands++;
		}
	}

	// creating links between islands (from centroid point, remove Island variable)
	if (Islands > 0)
	{
		TArray<FVector> Links;
		for (int i = 0; i< IslandCentroids.Num() - 1; i++)
		{
			Links.Append(JoiningIslands(IslandCentroids[i], IslandCentroids[i+1]));

			// search grid for tiles to change into links
			for (auto& Link : Links)
			{
				if (FTilePropertiesStruct* TileToChange = GridTiles.Find(Link))
				{
					FTilePropertiesStruct NewStatus;
					NewStatus.WorldLocation = TileToChange->WorldLocation;
					//NewStatus.TileStates = PathTag;
					NewStatus.TileTags.Reset();
					NewStatus.TileTags.AddTag(TagToFind);

					GridTiles.Add(Link, NewStatus);
				}
			}

			Links.Empty();
		}

		
	}
	
	return Islands;
}

bool UTileDirectionUtils::IsTileSafe(TMap<FVector, FTilePropertiesStruct>& GridTiles, FVector TileToVisit,
	TArray<TArray<bool>> &Visited, FGameplayTag TagToFind, FVector2D GridSize)
{
	
	if (IsTileBeforeBoundary(GridSize.X, GridSize.Y, TileToVisit))
	{
		FTilePropertiesStruct* VisitedTile = GridTiles.Find(TileToVisit);
		if (VisitedTile && VisitedTile->TileTags.HasTag(TagToFind) && !Visited[TileToVisit.X][TileToVisit.Y])
		{
			return true;
		}
	}
	return false;
}

// detects all adjoining tiles forming an island
FVector UTileDirectionUtils::BFS(TMap<FVector, FTilePropertiesStruct>& GridTiles, FVector TileToVisit,
	TArray<TArray<bool>>& Visited, FGameplayTag TagToFind, FVector2D GridSize)
{
	TQueue<FVector> VisitedQueue;
	VisitedQueue.Enqueue(TileToVisit);
	Visited[TileToVisit.X][TileToVisit.Y] = true;
	TArray<FVector> IslandTiles;

	// exploring all adjacent tiles
	while (!VisitedQueue.IsEmpty())
	{
		FVector CurrentTile ;
		if (!VisitedQueue.Dequeue(CurrentTile)) break;
		
		FVector NeighbourTile;
		int enumNum = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
		for (int i = 0; i < enumNum; i++)
		{
			switch (StaticCast<ETileNeighbour>(i))
			{
			case ETileNeighbour::North:
				NeighbourTile = UTileDirectionUtils::NorthNeighbourCoords(CurrentTile);
				break;
			case ETileNeighbour::Northeast:
				NeighbourTile = UTileDirectionUtils::NorthEastNeighbourCoords(CurrentTile);
				break;
			case ETileNeighbour::Southeast:
				NeighbourTile = UTileDirectionUtils::SouthEastNeighbourCoords(CurrentTile);
				break;
			case ETileNeighbour::South:
				NeighbourTile = UTileDirectionUtils::SouthNeighbourCoords(CurrentTile);
				break;
			case ETileNeighbour::Southwest:
				NeighbourTile = UTileDirectionUtils::SouthWestNeighbourCoords(CurrentTile);
				break;
			default:
				NeighbourTile = UTileDirectionUtils::NorthWestNeighbourCoords(CurrentTile);
				break;
			}

			// if tile is a specified tile type add to queue and mark visited
			if (IsTileSafe(GridTiles, NeighbourTile, Visited, TagToFind, GridSize))
			{
				//mark as visited, add to queue
				Visited[NeighbourTile.X][NeighbourTile.Y] = true;
				VisitedQueue.Enqueue(NeighbourTile);

				// saving location
				IslandTiles.Emplace(NeighbourTile);
			}
		}
	}

	// save locations of all tiles in islands
	// calculate centroid of island
	// return this value.
	FVector SumOfTiles = FVector::ZeroVector;
	for (FVector Tile : IslandTiles)
	{
		SumOfTiles += Tile;
	}

	//returning centroid of island
	
	const FVector IslandCentreF = SumOfTiles / IslandTiles.Num();
	const FVector IslandCenterInt (FMath::RoundToInt(IslandCentreF.X), FMath::RoundToInt(IslandCentreF.Y),0);
	return IslandCenterInt;
	
	
	// returning random tile within island
	//return IslandTiles[FMath::RandRange(0, IslandTiles.Num() - 1)];
	
}

TArray<FVector> UTileDirectionUtils::JoiningIslands(const FVector& TileA, const FVector& TileB)
{
	TArray<FVector> IslandLinks;

	const FIntVector A = EvenQToCube(TileA);
	const FIntVector B = EvenQToCube(TileB);

	// will need check outwidth function to detect no links formed
	const int N = CubeDistance(A,B);
	if (N == 0)
	{
		IslandLinks.Add(TileA);
		return IslandLinks;
	}

	// nudging endpoint to avoid boundary rounding artifacts during interpolation
	const FVector AF = CubeToFloat(A) + FVector(1e-6f, 2e-6f, -3e-6f);
	const FVector BF = CubeToFloat(B) + FVector(1e-6f, 2e-6f, -3e-6f);

	IslandLinks.Reserve(N + 1);
	for (int i = 1; i <= N; i++)
	{
		const float T = static_cast<float>(i) / static_cast<float>(N);
		const FVector LerpC = FMath::Lerp(AF, BF, T);
		const FIntVector C = CubeRound(LerpC);
		const FVector Qr = CubeToEvenQ(C);

		// checking for any duplicates, only adding links if not already added
		if (IslandLinks.Num() == 0 || IslandLinks.Last() != Qr)
		{
			IslandLinks.Add(Qr);
		}
	}

	return IslandLinks;
}



