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
	
	if (CentralState && NWNeighbourState && NENeighbourState)
	{
		CentralStateIsTag = CentralState->TileTags.HasAny(CellConfig.TagsToCheck);
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
