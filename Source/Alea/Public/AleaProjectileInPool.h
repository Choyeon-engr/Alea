#pragma once

#include "CoreMinimal.h"
#include "AleaActorInPool.h"
#include "AleaProjectileInPool.generated.h"

UCLASS()
class ALEA_API AAleaProjectileInPool : public AAleaActorInPool
{
	GENERATED_BODY()

public:
	AAleaProjectileInPool();

	void Initialize(UParticleSystem* AttackEffect, class USoundCue* AttackSound, UParticleSystem* Hit, class AAleaCharacter* Spwnr, AAleaCharacter* Trgt, float Damage);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	void Release();

	class USphereComponent* SphereCollision;
	class UParticleSystemComponent* Effect;
	class UAudioComponent* Audio;

	UParticleSystem* HitEffect;

	AAleaCharacter* Spawner;
	AAleaCharacter* Target;

	float AttackDamage;
};
