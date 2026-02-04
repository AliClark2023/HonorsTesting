#include "TileDirectionUtils.h"

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
FVector UTileDirectionUtils::GetNeighbourPos(const ETileNeighbour Neighbour, const FVector& CurrentTile)
{
	switch (Neighbour)
	{
	case ETileNeighbour::North:
		return  UTileDirectionUtils::NorthNeighbourCoords(CurrentTile);
	case ETileNeighbour::Northeast:
		return  UTileDirectionUtils::NorthEastNeighbourCoords(CurrentTile);
	case ETileNeighbour::Southeast:
		return  UTileDirectionUtils::SouthEastNeighbourCoords(CurrentTile);
	case ETileNeighbour::South:
		return  UTileDirectionUtils::SouthNeighbourCoords(CurrentTile);
	case ETileNeighbour::Southwest:
		return  UTileDirectionUtils::SouthWestNeighbourCoords(CurrentTile);
	case ETileNeighbour::Northwest:
		return  UTileDirectionUtils::NorthWestNeighbourCoords(CurrentTile);
	default:
		return UTileDirectionUtils::NorthNeighbourCoords(CurrentTile);
	}
}
*/

/*
 * Following Functions assume Even-Q hex grid
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

