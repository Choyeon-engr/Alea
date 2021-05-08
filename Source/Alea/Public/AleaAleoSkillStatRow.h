#pragma once

#include "Engine/DataTable.h"
#include "AleaAleoSkillStatRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaAleoSkillStatRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE float GetRange() const noexcept { return Range; }
	FORCEINLINE float GetAttackDamage() const noexcept { return AttackDamage; }
	FORCEINLINE float GetCoolDownTime() const noexcept { return CoolDownTime; }
	FORCEINLINE float GetManaCost() const noexcept { return ManaCost; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float CoolDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float ManaCost;
};
