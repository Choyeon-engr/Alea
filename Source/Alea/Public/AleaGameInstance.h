#pragma once

#include "Engine/GameInstance.h"
#include "AleaGameInstance.generated.h"

UCLASS()
class ALEA_API UAleaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UAleaGameInstance();

	class AAleaActorPool* GetActorPool();

private:
	TSubclassOf<AAleaActorPool> ActorPoolClass;
	AAleaActorPool* ActorPool;
};
