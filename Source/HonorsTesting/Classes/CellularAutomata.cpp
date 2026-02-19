#include "CellularAutomata.h"


TPair<bool,FTilePropertiesStruct> UCellularAutomata::Rule30(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, 
		const FVector &CentralTile, const FCellularConfig &CellConfig)
{
	FTilePropertiesStruct NewState;
	const FTilePropertiesStruct* CentralState;
	const FTilePropertiesStruct* NWNeighbourState;
	const FTilePropertiesStruct* NENeighbourState;
	
	bool NewStateIsTag = false;
	
	bool CentralStateIsTag = false;
	bool LeftStateIsTag = false;
	bool RightStateIsTag = false;
	
	// obtaining tile states (Tags, tile properties) (already contains built-in boundary response)
	CentralState = GridRef.Find(CentralTile);
	TPair<FVector, bool> NWNeighbour = UTileDirectionUtils::NorthWestNeighbour(GridRef, GridSize, CentralTile);
	TPair<FVector, bool> NENeighbour = UTileDirectionUtils::NorthEastNeighbour(GridRef, GridSize, CentralTile);
	
	NWNeighbourState = GridRef.Find(NWNeighbour.Key);
	NENeighbourState = GridRef.Find(NENeighbour.Key);
	
	// valid neighbour states
	if (CentralState && NWNeighbourState && NENeighbourState)
	{
		// checking if neighbour coords are on boundary (making them false if they are)
		CentralStateIsTag = CentralState->TileTags.HasAny(CellConfig.TagsToCheck);
		//NWNeighbour.Value ? LeftStateIsTag = NWNeighbourState->TileTags.HasAny(CellConfig.TagsToCheck): LeftStateIsTag = false;
		//NENeighbour.Value ? RightStateIsTag = NENeighbourState->TileTags.HasAny(CellConfig.TagsToCheck): RightStateIsTag = false;
		
		
		LeftStateIsTag = NWNeighbourState->TileTags.HasAny(CellConfig.TagsToCheck);
		RightStateIsTag = NENeighbourState->TileTags.HasAny(CellConfig.TagsToCheck);
		
		
		// state calculation based on neighbouring states
		// Formula new state (is tag specified): leftState != (CentralState || RightState)
		
		NewState.WorldLocation = CentralState->WorldLocation;
		
		NewStateIsTag = LeftStateIsTag != (CentralStateIsTag || RightStateIsTag);
		if (NewStateIsTag)
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToApply);
		}else
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToRevert);
		}
		return TPair<bool,FTilePropertiesStruct>(true, NewState);
	}

	return TPair<bool,FTilePropertiesStruct>(false, NewState);
}

// applies specified sequence for state comparisons
TPair<bool, FTilePropertiesStruct> UCellularAutomata::Wolfram(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CentralTile, const FCellularConfig& CellConfig)
{
	// make getter functions to return FTileProperties based on direction and starting tile
	FTilePropertiesStruct NewState;
	bool NewStateIsTag = false;
	const FTilePropertiesStruct* CentralState;
	const FTilePropertiesStruct* NWNeighbourState;
	const FTilePropertiesStruct* NENeighbourState;
	
	bool CentralStateIsTag ;
	bool LeftStateIsTag;
	bool RightStateIsTag;
	
	// obtaining tile states (Tags, tile properties) (already contains built-in boundary response)
	CentralState = GridRef.Find(CentralTile);
	TPair<FVector, bool> NWNeighbour = UTileDirectionUtils::NorthWestNeighbour(GridRef, GridSize, CentralTile);
	TPair<FVector, bool> NENeighbour = UTileDirectionUtils::NorthEastNeighbour(GridRef, GridSize, CentralTile);
	
	NWNeighbourState = GridRef.Find(NWNeighbour.Key);
	NENeighbourState = GridRef.Find(NENeighbour.Key);

	
	if (CentralState && NWNeighbourState && NENeighbourState)
	{
		LeftStateIsTag = NWNeighbourState->TileTags.HasAny(CellConfig.TagsToCheck);
		CentralStateIsTag = CentralState->TileTags.HasAny(CellConfig.TagsToCheck);
		RightStateIsTag = NENeighbourState->TileTags.HasAny(CellConfig.TagsToCheck);
		
		// compare all tile states for corresponding code sequence, apply new state when matches
		for (const auto& Sequence : CellConfig.CodeSequence.Segments)
		{
			if (Sequence.Left == LeftStateIsTag
				&& Sequence.Centre == CentralStateIsTag
				&& Sequence.Right == RightStateIsTag)
			{
				NewStateIsTag = Sequence.NewState;
				break;
			}
		}
		// applying tags depending on new state
		if (NewStateIsTag)
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToApply);
		}else
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToRevert);
		}
		return TPair<bool,FTilePropertiesStruct>(true, NewState);
	}

	return TPair<bool,FTilePropertiesStruct>(false, NewState);
}

TPair<bool, FTilePropertiesStruct> UCellularAutomata::GameOfLife(const TMap<FVector, FTilePropertiesStruct>& GridRef,
	const FIntVector2& GridSize, const FVector& CentralTile, const FCellularConfig& CellConfig)
{
	FTilePropertiesStruct NewState;
	int AliveNeighbours = 0;
	
	// adding all neighbours to array for checking
	TArray<ETileNeighbour> Neighbours;
	for (int32 i = 0; i < StaticEnum<ETileNeighbour>()->NumEnums() - 1; i++)
	{
		const auto EnumVal = static_cast<ETileNeighbour>(i);
		if (!StaticEnum<ETileNeighbour>()->HasMetaData(TEXT("Hidden"), i))
		{
			Neighbours.Add(EnumVal);
		}
	}
	
	// check all surrounding neighbours tags and increment alive neighbours
	
	for (const auto& Neighbour : Neighbours)
	{
		TPair<FVector, bool> NeighbourCoords = UTileDirectionUtils::GetNeighbour(GridRef, GridSize, Neighbour, CentralTile);
		if (NeighbourCoords.Value)
		{
			if (GridRef.Find(NeighbourCoords.Key)->TileTags.HasAny(CellConfig.TagsToCheck)) AliveNeighbours++;
		}
	}
	// check central tile status
	// if central is alive (Path)
		// but neighbour count is less than death limit, kill central (revert tag)
	if (GridRef.Find(CentralTile)->TileTags.HasAny(CellConfig.TagsToCheck))
	{
		if (AliveNeighbours < CellConfig.GameOfLifeConfig.StarvationLimit)
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToRevert);
		}else
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToApply);
		}
	}
	// if central is dead (Non-Path)
	// but neighbour count is greater than birth rate, become alive
	else
	{
		if (AliveNeighbours > CellConfig.GameOfLifeConfig.BirthNumber)
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToApply);
		}else
		{
			NewState.TileTags.Reset();
			NewState.TileTags.AddTag(CellConfig.TagToRevert);
		}
	}

	return TPair<bool,FTilePropertiesStruct>(true, NewState);
}

// returns array of tile coords that are deemed to be alive (paths)
TArray<FVector> UCellularAutomata::RandomPopulate(const TMap<FVector, FTilePropertiesStruct> &GridRef, const FIntVector2 &GridSize, const int &Chance)
{
	TArray<FVector> Populated;
	
	for (auto &Tile: GridRef)
	{
		if (UTileDirectionUtils::IsTileOnBoundary(GridSize.X, GridSize.Y, Tile.Key)) continue;
		
		int TileAliveChance = FMath::RandRange(1, 100);
		if (TileAliveChance <= Chance)
		{
			Populated.Add(Tile.Key);
		}
	}
	return Populated;
}
