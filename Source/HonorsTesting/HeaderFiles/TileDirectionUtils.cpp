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

