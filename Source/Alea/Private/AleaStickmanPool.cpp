#include "AleaStickmanPool.h"
#include "AleaStickman.h"

void AAleaStickmanPool::Release(AAleaStickman* Stickman)
{
	Pool.FindOrAdd(Stickman->GetClass()).Add(Stickman);
}

AAleaStickman* AAleaStickmanPool::Spawn(const TSubclassOf<AAleaStickman> Class, const FTransform& Transform)
{
	if (Class)
	{
		if (const auto PoolPtr = Pool.Find(Class))
		{
			while (PoolPtr->Num() > 0)
			{
				if (const auto Stickman = PoolPtr->Pop().Get())
				{
					Stickman->SetActorTransform(Transform);
					Stickman->Activate();

					return Stickman;
				}
			}
		}

		if (const auto Stickman = Cast<AAleaStickman>(GetWorld()->SpawnActor(Class, &Transform)))
		{
			Stickman->SetPool(this);
			Stickman->Activate();

			return Stickman;
		}
	}

	return nullptr;
}
