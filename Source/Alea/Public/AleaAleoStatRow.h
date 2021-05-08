#pragma once

#include "Engine/DataTable.h"
#include "AleaAleoStatRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaAleoStatRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE float GetHealth() const noexcept { return Health; }
	FORCEINLINE float GetHealthRegen() const noexcept { return HealthRegen; }
	FORCEINLINE float GetMana() const noexcept { return Mana; }
	FORCEINLINE float GetManaRegen() const noexcept { return ManaRegen; }
	FORCEINLINE float GetMovementSpeed() const noexcept { return MovementSpeed; }
	FORCEINLINE float GetRespawnWaitTime() const noexcept { return RespawnWaitTime; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float HealthRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float ManaRegen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float RespawnWaitTime;
};
