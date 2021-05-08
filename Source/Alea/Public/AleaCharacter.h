#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AleaCharacter.generated.h"

UCLASS()
class ALEA_API AAleaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAleaCharacter();

	FORCEINLINE bool IsDead() const noexcept { return bDead; }
	FORCEINLINE AAleaCharacter* GetTarget() const noexcept { return TrgtAutoAttacked; }

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FName Team;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRangedAttack(UParticleSystem* AttackEffect, class USoundCue* AttackSound, UParticleSystem* Hit, class AAleaCharacter* Spwnr, AAleaCharacter* Trgt, float Damage);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDie();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDamageText(float FinalDamage);

	UFUNCTION(Server, Reliable)
	void ServerPlayDamageText(float FinalDamage, uint8 PlayerNumber);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayDamageText(float FinalDamage, uint8 PlayerNumber);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDroppedChipsText();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayDroppedChipsText(uint8 PlayerNumber);

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bDead : 1;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(Replicated)
	AAleaCharacter* TrgtAutoAttacked;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWidgetComponent* DamageTextWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWidgetComponent* DroppedChipsTextWidget;

	float AutoAttackCoolDownTime;

	class AAleaActorPool* ActorPool;
};
