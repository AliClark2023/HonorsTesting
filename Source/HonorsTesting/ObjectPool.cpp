#include "HeaderFiles/ObjectPool.h"

AObjectPool::AObjectPool()
{
}

void AObjectPool::BeginPlay()
{
	Super::BeginPlay();
}

void AObjectPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObjectPool::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AObjectPool::SetInUse(const bool used){
	inUse = used;
	SetActorEnableCollision(used);
	SetActorHiddenInGame(!used);
	SetActorTickEnabled(used);
	
	GetWorldTimerManager().ClearTimer(MyTimerHandle);
	if(used)
	{
		GetWorldTimerManager().SetTimer(MyTimerHandle, this, &AObjectPool::ReturnToPool,  timeToLive, false);
	}
	
}

void AObjectPool::ReturnToPool(){
	SetInUse(false);
}