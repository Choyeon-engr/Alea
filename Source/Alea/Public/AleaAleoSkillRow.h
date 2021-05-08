#pragma once

#include "Engine/DataTable.h"
#include "AleaAleoSkillRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaAleoSkillRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE UDataTable* GetAleoSkillStatTable() const noexcept { return AleoSkillStatTable; }
	FORCEINLINE bool IsMeleeAttack() const noexcept { return bMeleeAttack; }
	FORCEINLINE class UParticleSystem* GetAttackEffect() const noexcept { return AttackEffect; }
	FORCEINLINE class USoundCue* GetAttackSound() const noexcept { return AttackSound; }
	FORCEINLINE UParticleSystem* GetHitEffect() const noexcept { return HitEffect; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UDataTable* AleoSkillStatTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UTexture2D* SkillImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	uint8 bMeleeAttack : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* HitEffect;
};
