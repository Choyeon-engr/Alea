#include "AleaActorPool.h"
#include "AleaActorInPool.h"

#include "Engine/World.h"

void AAleaActorPool::Release(AAleaActorInPool* Actor)
{
	Pool.FindOrAdd(Actor->GetClass()).Add(Actor);
}

AAleaActorInPool* AAleaActorPool::Spawn(const TSubclassOf<AAleaActorInPool> Class, const FTransform& Transform)
{
	if (Class)
	{
		if (const auto PoolPtr = Pool.Find(Class))
		{
			while (PoolPtr->Num() > 0)
			{
				if (const auto Actor = PoolPtr->Pop().Get())
				{
					Actor->SetActorTransform(Transform);
					Actor->Activate();

					return Actor;
				}
			}
		}
		
		if (const auto Actor = Cast<AAleaActorInPool>(GetWorld()->SpawnActor(Class, &Transform)))
		{
			Actor->SetPool(this);
			Actor->Activate();

			return Actor;
		}
	}

	return nullptr;
}
