# include "DrunkardWalk.h"

TPair<bool,FIntVector2> UDrunkardWalk::Walk(const TMap<FVector, FTilePropertiesStruct>& GridRef, const FIntVector2& GridSize, const FIntVector2& StartPoint,
	const FGameplayTagContainer& TagsToFind, const EDlaType& WalkType)
{
	
	int32 MaxSteps = 5000; // tune as needed
	int32 Steps = 0;

	// make sure this variable is set after every invocation
	FIntVector2 CurrentLocation = StartPoint;
	//resulting tile
	TPair<bool,FIntVector2> Result = TPair<bool,FIntVector2>(false,FIntVector2(0,0));
	
	// Neighbor Selection (make static function to use in main DW algorithm?)
	TArray<ETileNeighbour> Neighbours;
	for (int32 i = 0; i < StaticEnum<ETileNeighbour>()->NumEnums() - 1; i++)
	{
		const auto EnumVal = static_cast<ETileNeighbour>(i);
		/*
		if (!StaticEnum<ETileNeighbour>()->HasMetaData(TEXT("Hidden"), i))
		{
			
		}
		*/
		Neighbours.Add(EnumVal);
	}
	
	while (!Result.Key && Steps < MaxSteps)
	{
		Steps++;
		// Shuffle neighbors instead of random‑re-rolling, add to old DW method
		Neighbours.Sort([](auto&, auto&) {
			return FMath::RandBool();
		});

		bool Moved = false;

		for (ETileNeighbour N : Neighbours)
		{
			FVector WalkerLocation(CurrentLocation.X, CurrentLocation.Y, 0);
			TPair<FVector,bool> NeighbourPos = 
				UTileDirectionUtils::GetNeighbour(GridRef,GridSize,N,WalkerLocation);

			const FTilePropertiesStruct* Tile = GridRef.Find(NeighbourPos.Key);

			if (!NeighbourPos.Value || !Tile)
				continue;

			// Found target tag
			if (Tile->TileTags.HasAny(TagsToFind))
			{
				

				switch (WalkType)
				{
				case EDlaType::Inwards:
					Result = TPair<bool,FIntVector2>(true, CurrentLocation);
					break;
				case EDlaType::Outwards:
					Result = TPair<bool,FIntVector2>(true, FIntVector2(NeighbourPos.Key.X, NeighbourPos.Key.Y));
					break;
				default: // Central
					Result = TPair<bool,FIntVector2>(true, CurrentLocation);
					break;
				}
				
				//Result = TPair<bool,FIntVector2>(true, CurrentLocation);
				return Result;
			}

			// Move into neighbor
			CurrentLocation = FIntVector2(
				static_cast<int>(NeighbourPos.Key.X),
				static_cast<int>(NeighbourPos.Key.Y)
			);

			Moved = true;
			break;
		}

		// No valid neighbor found breaks walk
		if (!Moved)
			break;
	}

	return Result;
	
}
