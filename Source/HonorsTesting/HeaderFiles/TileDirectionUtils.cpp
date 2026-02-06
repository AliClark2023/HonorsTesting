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


// determining if grid contains islands

int UTileDirectionUtils::CountIslands(TMap<FVector, FTilePropertiesStruct>& GridTiles, FVector2D GridSize, FGameplayTag TagToFind)
{
	int n = GridSize.X;
	int m = GridSize.Y;
	int Islands = 0;
	
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
			BFS(GridTiles, Tile.Key, Visited, TagToFind, GridSize);
			Islands++;
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

void UTileDirectionUtils::BFS(TMap<FVector, FTilePropertiesStruct>& GridTiles, FVector TileToVisit,
	TArray<TArray<bool>>& Visited, FGameplayTag TagToFind, FVector2D GridSize)
{
	TQueue<FVector> VisitedQueue;
	VisitedQueue.Enqueue(TileToVisit);
	Visited[TileToVisit.X][TileToVisit.Y] = true;

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
		
			if (IsTileSafe(GridTiles, NeighbourTile, Visited, TagToFind, GridSize))
			{
				//mark as visited, add to queue, mark as visited
				Visited[NeighbourTile.X][NeighbourTile.Y] = true;
				VisitedQueue.Enqueue(NeighbourTile);
			}
		}
	}
	
}



