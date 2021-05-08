#include "AleaActorInPool.h"
#include "AleaActorPool.h"

void AAleaActorInPool::Release()
{
	if (!bActive)
	{
		return;
	}

	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	bActive = false;
	Pool->Release(this);
}

void AAleaActorInPool::Activate()
{
	if (bActive)
	{
		return;
	}

	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);

	bActive = true;
}
