#pragma once

#include "Engine/DataTable.h"
#include "AleaStickmanRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaStickmanRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE float GetHealth() const noexcept { return Health; }
	FORCEINLINE bool IsMeleeAttack() const noexcept { return bMeleeAttack; }
	FORCEINLINE float GetDetectRange() const noexcept { return DetectRange; }
	FORCEINLINE float GetAttackRange() const noexcept { return AttackRange; }
	FORCEINLINE float GetAttackDamage() const noexcept { return AttackDamage; }
	FORCEINLINE float GetCoolDownTime() const noexcept { return CoolDownTime; }
	FORCEINLINE float GetExperience() const noexcept { return Experience; }
	FORCEINLINE float GetExperienceRange() const noexcept { return ExperienceRange; }
	FORCEINLINE int GetChips() const noexcept { return Chips; }
	FORCEINLINE class UParticleSystem* GetAttackEffect() const noexcept { return AttackEffect; }
	FORCEINLINE UParticleSystem* GetHitEffect() const noexcept { return HitEffect; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	uint8 bMeleeAttack : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float DetectRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float CoolDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float ExperienceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int Chips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* HitEffect;
};
