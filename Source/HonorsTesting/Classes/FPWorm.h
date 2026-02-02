#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "GameplayTagContainer.h"
#include "FPWorm.generated.h"

// Perlin Worm Class for producing paths within the grid
class FFPWorm
{
public:
	FFPWorm(const FVector2D Coord, const FGameplayTag Tag)
	{
		X = Coord.X;
		Y = Coord.Y;
		Type = Tag;
		Segments.Push(Coord);
		Length = Segments.Num();
	};
	
	void Grow(const FVector2D Coord)
	{
		Segments.Push(Coord);
		Length = Segments.Num();
		X = Coord.X;
		Y = Coord.Y;
	};
	
	TArray<FVector2D> Display(){return Segments;};
	
private:
	int X,Y,Length;
	TArray<FVector2D> Segments;
	FGameplayTag Type;
};
