#include "AleaGameInstance.h"
#include "AleaActorPool.h"

#include "Engine/World.h"

UAleaGameInstance::UAleaGameInstance()
	: ActorPoolClass{ AAleaActorPool::StaticClass() }
{
}

AAleaActorPool* UAleaGameInstance::GetActorPool()
{
	if (!ActorPool)
	{
		ActorPool = Cast<AAleaActorPool>(GetWorld()->SpawnActor(ActorPoolClass));
	}

	return ActorPool;
}
