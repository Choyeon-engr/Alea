#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AleaActorPool.generated.h"

UCLASS()
class ALEA_API AAleaActorPool : public AActor
{
	GENERATED_BODY()
	
public:
	void Release(class AAleaActorInPool* Actor);

	UFUNCTION(BlueprintCallable)
	AAleaActorInPool* Spawn(const TSubclassOf<AAleaActorInPool> Class, const FTransform& Transform);

	template <class T>
	T* Spawn(const TSubclassOf<AAleaActorInPool> Class, const FTransform& Transform)
	{
		static_assert(TIsDerivedFrom<T, AAleaActorInPool>::IsDerived, "");
		return Cast<T>(Spawn(Class, Transform));
	}

private:
	TMap<TSubclassOf<AAleaActorInPool>, TArray<TWeakObjectPtr<AAleaActorInPool>>> Pool;
};
