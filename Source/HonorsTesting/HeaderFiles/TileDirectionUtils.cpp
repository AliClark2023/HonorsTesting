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

TPair<FVector, bool> UTileDirectionUtils::GetNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize,
	const ETileNeighbour& Neighbour, const FVector& CurrentTile)
{
	switch (Neighbour)
	{
	case ETileNeighbour::North:
		return  NorthNeighbour(GridRef, GridSize, CurrentTile);
	case ETileNeighbour::Northeast:
		return  NorthEastNeighbour(GridRef, GridSize, CurrentTile);
	case ETileNeighbour::Southeast:
		return  SouthEastNeighbour(GridRef, GridSize, CurrentTile);
	case ETileNeighbour::South:
		return  SouthNeighbour(GridRef, GridSize, CurrentTile);
	case ETileNeighbour::Southwest:
		return  SouthWestNeighbour(GridRef, GridSize, CurrentTile);
	case ETileNeighbour::Northwest:
		return  NorthWestNeighbour(GridRef, GridSize, CurrentTile);
	default:
		return TPair<FVector, bool>(CurrentTile, false);
	}
}

TPair<FVector, bool> UTileDirectionUtils::NorthNeighbour(const TMap<FVector, FTilePropertiesStruct> &GridRef, 
	const FIntVector2 &GridSize, const FVector& CurrentTile)
{
	constexpr ETileNeighbour Type = ETileNeighbour::North;
	FVector TileNeighbour(NorthNeighbourCoords(CurrentTile));

	if (!GridRef.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (IsTileOnBoundary(GridSize.X, GridSize.Y, TileNeighbour))
	{
		return TPair<FVector, bool>(GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

TPair<FVector, bool> UTileDirectionUtils::NorthEastNeighbour(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CurrentTile)
{
	constexpr ETileNeighbour Type = ETileNeighbour::Northeast;
	FVector TileNeighbour(NorthEastNeighbourCoords(CurrentTile));
	
	if (!GridRef.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (IsTileOnBoundary(GridSize.X, GridSize.Y, TileNeighbour))
	{
		return TPair<FVector, bool>(GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

TPair<FVector, bool> UTileDirectionUtils::SouthEastNeighbour(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CurrentTile)
{
	constexpr ETileNeighbour Type = ETileNeighbour::Southeast;
	FVector TileNeighbour(SouthEastNeighbourCoords(CurrentTile));


	if (!GridRef.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (IsTileOnBoundary(GridSize.X, GridSize.Y, TileNeighbour))
	{
		return TPair<FVector, bool>(GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

TPair<FVector, bool> UTileDirectionUtils::SouthNeighbour(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CurrentTile)
{
	constexpr ETileNeighbour Type = ETileNeighbour::South;
	FVector TileNeighbour(SouthNeighbourCoords(CurrentTile));
	if (!GridRef.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (IsTileOnBoundary(GridSize.X, GridSize.Y, TileNeighbour))
	{
		return TPair<FVector, bool>(GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

TPair<FVector, bool> UTileDirectionUtils::SouthWestNeighbour(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CurrentTile)
{
	constexpr ETileNeighbour Type = ETileNeighbour::Southwest;
	FVector TileNeighbour(SouthWestNeighbourCoords(CurrentTile));

	if (!GridRef.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (IsTileOnBoundary(GridSize.X, GridSize.Y,TileNeighbour))
	{
		return TPair<FVector, bool>(GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

TPair<FVector, bool> UTileDirectionUtils::NorthWestNeighbour(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CurrentTile)
{
	constexpr ETileNeighbour Type = ETileNeighbour::Northwest;
	FVector TileNeighbour(NorthWestNeighbourCoords(CurrentTile));
	
	if (!GridRef.Contains(TileNeighbour))
	{
		return TPair<FVector, bool>(CurrentTile, false);
	}else if (IsTileOnBoundary(GridSize.X, GridSize.Y, TileNeighbour))
	{
		return TPair<FVector, bool>(GetOppositeNeighbour(Type,CurrentTile), true);
	}
	return TPair<FVector, bool>(TileNeighbour, true);
}

// returns position of opposite neighbour, used for responding to boundaries
FVector UTileDirectionUtils::GetOppositeNeighbour(ETileNeighbour CurrentNeighbour, const FVector& CurrentTile )
{
	switch (CurrentNeighbour)
	{
	case ETileNeighbour::North:
		return  SouthNeighbourCoords(CurrentTile);
	case ETileNeighbour::Northeast:
		return  SouthWestNeighbourCoords(CurrentTile);
	case ETileNeighbour::Southeast:
		return  NorthWestNeighbourCoords(CurrentTile);
	case ETileNeighbour::South:
		return  NorthNeighbourCoords(CurrentTile);
	case ETileNeighbour::Southwest:
		return  NorthEastNeighbourCoords(CurrentTile);
	case ETileNeighbour::Northwest:
		return  SouthEastNeighbourCoords(CurrentTile);
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

FIntVector UTileDirectionUtils::CubeSubtract(const FIntVector& A, const FIntVector& B)
{
	return FIntVector(A.X - B.X, A.Y - B.Y, A.Z - B.Z);
}

//sourced from: https://www.redblobgames.com/grids/hexagons/#conversions
// converts Even Q coords to Cube coordinates 
FIntVector UTileDirectionUtils::EvenQToCube(const FVector& EvenQ)
{
/*
	const int Parity = static_cast<int>(EvenQ.X) & 1;
	const int Q = EvenQ.X;
	const int Y = EvenQ.Y - (Q + Parity) / 2;
	
	return FIntVector(Q, Y, -Q-Y);
	*/
	// other method

	const int Col = EvenQ.X;
	const int Row = EvenQ.Y;

	const int X = Col;
	const int Z = Row - (Col - (Col & 1)) / 2;
	const int Y = -X - Z;

	return FIntVector(X, Y, Z);


}

FVector UTileDirectionUtils::CubeToEvenQ(const FIntVector& Cube)
{
/*
	const int Parity = Cube.X & 1;
	const int Col = Cube.X;
	const int Row = Cube.Y + (Cube.X + Parity) / 2;
	
	return FVector(Col, Row, 0);
*/
	// other method

	const int Col = Cube.X;
	const int Row = Cube.Z + (Col - (Col & 1)) / 2;

	return FVector(Col, Row, 0);

}

// manhattan distance in cube space
int UTileDirectionUtils::CubeDistance(const FIntVector& A, const FIntVector& B)
{
	const FIntVector Vec = CubeSubtract(A,B);
	return (FMath::Abs(Vec.X) + FMath::Abs(Vec.Y) + FMath::Abs(Vec.Z)) / 2;
}

//rounding float cube back to int cube
FIntVector UTileDirectionUtils::CubeRound(const FVector& FracCoords)
{
	int Q = FMath::RoundToInt(FracCoords.X);
	int R = FMath::RoundToInt(FracCoords.Y);
	int S = FMath::RoundToInt(FracCoords.Z);

	const float QDiff = FMath::Abs(Q - FracCoords.X);
	const float RDiff = FMath::Abs(R - FracCoords.Y);
	const float SDiff = FMath::Abs(S - FracCoords.Z);
	
	if (QDiff > RDiff && QDiff > SDiff)
	{
		Q = -R-S;
	}else if (RDiff > SDiff)
	{
		R = -Q-S;
	}else
	{
		S = -Q-R;
	}
	
	return FIntVector(Q, R, S);
}

float UTileDirectionUtils::Lerp(float PointA, float PointB, float Fraction)
{
	return PointA + (PointB - PointA) * Fraction;
}

FVector UTileDirectionUtils::CubeLerp(FIntVector PointA, FIntVector PointB, float Fraction)
{
	return FVector(
		Lerp(PointA.X, PointB.X, Fraction),
		Lerp(PointA.Y, PointB.Y, Fraction),
		Lerp(PointA.Z, PointB.Z, Fraction)
		);
}

TArray<FIntVector> UTileDirectionUtils::CubeLineDraw(const FIntVector& TileA, const FIntVector& TileB)
{
	int N = CubeDistance(TileA, TileB);
	TArray<FIntVector> LineDraw;
	
	for (int i = 0; i <= N; i++)
	{
		float t = (N ==0 ? 0.0f : i / static_cast<float>(N));
		LineDraw.Add(CubeRound(CubeLerp(TileA, TileB, t)));
	}
	
	return LineDraw;
}


// determining if grid contains islands and joins any detected
int UTileDirectionUtils::CountIslands(TMap<FVector, FTilePropertiesStruct>& GridTiles, FVector2D GridSize, FGameplayTag TagToFind, FGameplayTagContainer ExcludeTags)
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
			// testing new method
			FIntVector TileA = EvenQToCube(IslandCentroids[i]);
			FIntVector TileB = EvenQToCube(IslandCentroids[i+1]);
			TArray<FIntVector> CubeLinks = CubeLineDraw(TileA, TileB);
			
			// break if function only returns 1 link (should be starting point)
			if (CubeLinks.Num() <= 1) break;
			
			for (auto Link : CubeLinks)
			{
				Links.Add(CubeToEvenQ(Link));
			}
			
			// search grid for tiles to change into links
			for (auto& Link : Links)
			{
				if (FTilePropertiesStruct* TileToChange = GridTiles.Find(Link))
				{
					// skip specified tags, prevents overwriting start/end or other specified tiles
					if (TileToChange->TileTags.HasAny(ExcludeTags)) continue;
					
					FTilePropertiesStruct NewStatus;
					NewStatus.WorldLocation = TileToChange->WorldLocation;
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
	
	
	// returning random tile within island ( can overflow )
	//return IslandTiles[FMath::RandRange(0, IslandTiles.Num() - 1)];
}



