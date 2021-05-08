#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AleaPlayerState.generated.h"

UCLASS()
class ALEA_API AAleaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	FName Team;

	UPROPERTY(Replicated)
	uint8 Number;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
