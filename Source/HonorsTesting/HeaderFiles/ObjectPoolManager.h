#pragma once
#include "ObjectPool.h"
#include "Components/ActorComponent.h"
#include "ObjectPoolManager.generated.h"

UCLASS (DefaultToInstanced, BlueprintType, Abstract,  
	   Meta=(ShortTooltip="An ActorComponent is a reusable component that can be added to any actor."),  
	   Config=Engine, MinimalAPI)  
class UAcObjectPoolManager: public UObject ,  
		public IInterface_AssetUserData ,  
		public IAsyncPhysicsStateProcessor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay();
public:
	/*
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Properties")
	UClassType<AObjectPool> PooledActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Properties")
	int PoolSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Properties")
	TArray<AObjectPool*> ObjectPool;

	UFUNCTION(BlueprintCallable, Category = "Pool Functions")
	void InitialzePool();
	*/
};

