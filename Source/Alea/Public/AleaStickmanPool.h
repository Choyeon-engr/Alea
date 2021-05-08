#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AleaStickmanPool.generated.h"

UCLASS()
class ALEA_API AAleaStickmanPool : public AActor
{
	GENERATED_BODY()
	
public:
	void Release(class AAleaStickman* Stickman);

	UFUNCTION(BlueprintCallable)
	AAleaStickman* Spawn(const TSubclassOf<AAleaStickman> Class, const FTransform& Transform);

private:
	TMap<TSubclassOf<AAleaStickman>, TArray<TWeakObjectPtr<AAleaStickman>>> Pool;
};
