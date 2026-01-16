#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPool.generated.h"

// pooled object base class
UCLASS()
class HONORSTESTING_API AObjectPool : public AActor
{
	GENERATED_BODY()
public:
	AObjectPool();
protected:
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION(BlueprintCallable, Category = "Pool Functions")
	void SetInUse(const bool used);
	UFUNCTION(BlueprintCallable, Category = "Pool Functions")
	void ReturnToPool();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Properties")
	bool inUse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Properties")
	float timeToLive;
private:
	FTimerHandle MyTimerHandle;
};
