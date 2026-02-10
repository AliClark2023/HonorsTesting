# include "DrunkardWalk.h"

TPair<bool,FIntVector2> UDrunkardWalk::Walk(const TMap<FVector, FTilePropertiesStruct>& GridRef, const FIntVector2& GridSize, const FIntVector2& StartPoint, const FGameplayTag TagToFind)
{
	// make sure this variable is set after every invocation
	FIntVector2 CurrentLocation = StartPoint;
	//resulting tile
	TPair<bool,FIntVector2> Result = TPair<bool,FIntVector2>(false,FIntVector2(0,0));
	
	// Neighbor Selection (make static function to use in main DW algorithm?)
	TArray<ETileNeighbour> VisitedTiles;
	const int MaxChoice = StaticEnum<ETileNeighbour>()->NumEnums() - 1;
	
	
	while (!Result.Key)
	{
		ETileNeighbour ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, MaxChoice));
		
		// re-selects another neighbor if already visited
		while (VisitedTiles.Contains(ChosenNeighbour))
		{
			if (VisitedTiles.Num() == MaxChoice)
			{
				// break current iteration
				return Result;
			}else
			{
				ChosenNeighbour = static_cast<ETileNeighbour>(FMath::RandRange(0, MaxChoice));
			}
		}
		
		VisitedTiles.Add(ChosenNeighbour);
	
		FVector WalkerLocation = FVector(CurrentLocation.X, CurrentLocation.Y, 0);
		TPair<FVector, bool> NeigbourPos = UTileDirectionUtils::GetNeighbour(GridRef,GridSize, ChosenNeighbour, WalkerLocation);

		if (const FTilePropertiesStruct* NeighbourType = GridRef.Find(NeigbourPos.Key); NeigbourPos.Value && NeighbourType)
		{
			// checking to specified Tag
			if (NeighbourType->TileTags.HasTag(TagToFind))
			{
				// return current location if neighbour is the specified tag
				Result = TPair<bool,FIntVector2>(true,CurrentLocation);
				return Result;
			}
			// progress into neighbouring tile and walk again
			CurrentLocation = FIntVector2(StaticCast<int>(NeigbourPos.Key.X), StaticCast<int>(NeigbourPos.Key.Y));
			VisitedTiles.Empty();
		}else
		{
			return Result;
		}
	}

	return Result;
}
