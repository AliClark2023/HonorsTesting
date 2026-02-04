#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "GameplayTagContainer.h"
#include "PerlinWorm.generated.h"

// Perlin Worm Struct for producing paths within the grid
USTRUCT(BlueprintType)
struct HONORSTESTING_API FPerlinWorm 
{
	GENERATED_BODY()
public:
	FPerlinWorm()
	{
		X = 0;
		Y = 0;
		Length = 0;
	};
	FPerlinWorm(const FVector2D Coord, const FGameplayTag Tag)
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

	//returns random point of worm segment
	FVector2D GetPointOnSegment()
	{
		return Segments[FMath::RandRange(0, Length - 1)];
	};
	
	TArray<FVector2D> Display(){return Segments;};
	int GetX() const {return X;};
	int GetY() const {return Y;};
	
private:
	int X,Y,Length;
	TArray<FVector2D> Segments;
	FGameplayTag Type;
};
