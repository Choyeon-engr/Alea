#pragma once

#include "GameFramework/Actor.h"
#include "AleaActorInPool.generated.h"

UCLASS()
class ALEA_API AAleaActorInPool : public AActor
{
	GENERATED_BODY()
	
public:
	void Release();
	void Activate();

	void SetPool(class AAleaActorPool* NewPool) { Pool = NewPool; }

	virtual void LifeSpanExpired() override { Release(); }

protected:
	uint8 bActive : 1;

private:
	AAleaActorPool* Pool;
};
