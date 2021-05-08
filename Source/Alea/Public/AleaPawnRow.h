#pragma once

#include "Engine/DataTable.h"
#include "AleaPawnRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaPawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE float GetHealth() const noexcept { return Health; }
	FORCEINLINE float GetRange() const noexcept { return Range; }
	FORCEINLINE float GetBaseDamage() const noexcept { return BaseDamage; }
	FORCEINLINE float GetAleoDmgRate() const noexcept { return AleoDmgRate; }
	FORCEINLINE float GetCardDmgRate() const noexcept { return CardDmgRate; }
	FORCEINLINE float GetDiceDmgRate() const noexcept { return DiceDmgRate; }
	FORCEINLINE float GetCoolDownTime() const noexcept { return CoolDownTime; }
	FORCEINLINE int GetChips() const noexcept { return Chips; }
	FORCEINLINE class UParticleSystem* GetAttackEffect() const noexcept { return AttackEffect; }
	FORCEINLINE class USoundCue* GetAttackSound() const noexcept { return AttackSound; }
	FORCEINLINE UParticleSystem* GetHitEffect() const noexcept { return HitEffect; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float AleoDmgRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float CardDmgRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float DiceDmgRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float CoolDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int Chips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* HitEffect;
};
