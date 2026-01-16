#include "HeaderFiles/ObjectPoolManager.h"

void UAcObjectPoolManager::BeginPlay()
{
	//InitialzePool();
	
}

/*
void UAcObjectPoolManager::InitialzePool()
{
	for (int i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector SpawnLocation(0,0,0);
		FRotator SpawnRotation(0,0,0);
		ObjectPool.Add(GetWorld()->SpawnActor(PooledActorClass, SpawnLocation, SpawnRotation, SpawnInfo));
	}
}
*/
