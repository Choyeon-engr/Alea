#pragma once

#include "Engine/DataTable.h"
#include "AleaTeamRow.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAleaTeamRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FORCEINLINE class UDataTable* GetStickmanTable() const noexcept { return StickmanTable; }
	FORCEINLINE UDataTable* GetPawnTable() const noexcept { return PawnTable; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UDataTable* StickmanTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UDataTable* PawnTable;
};
