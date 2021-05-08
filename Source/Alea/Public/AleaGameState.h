#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AleaGameState.generated.h"

UCLASS()
class ALEA_API AAleaGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void AddBlackTeamKillScore() { ++BlackTeamKillScore; }
	FORCEINLINE void AddWhiteTeamKillScore() { ++WhiteTeamKillScore; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 BlackTeamKillScore;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 WhiteTeamKillScore;
};
