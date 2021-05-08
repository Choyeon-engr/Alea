#pragma once

#include "Engine/DataTable.h"
#include "AleaAleoRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaAleoRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE UDataTable* GetAleoStatTable() const noexcept { return AleoStatTable; }
	FORCEINLINE UDataTable* GetAleoSkillTable() const noexcept { return AleoSkillTable; }
	FORCEINLINE class UParticleSystem* GetLevelUpEffect() const noexcept { return LevelUpEffect; }
	FORCEINLINE class USoundCue* GetLevelUpSound() const noexcept { return LevelUpSound; }
	FORCEINLINE UParticleSystem* GetRecallEffect() const noexcept { return RecallEffect; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UDataTable* AleoStatTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UDataTable* AleoSkillTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UTexture2D* ProfileImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UMaterial* ProfileMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* LevelUpEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USoundCue* LevelUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UParticleSystem* RecallEffect;
};
