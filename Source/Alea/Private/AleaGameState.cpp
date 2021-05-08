#include "AleaGameState.h"

#include "Net/UnrealNetwork.h"

void AAleaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAleaGameState, BlackTeamKillScore);
	DOREPLIFETIME(AAleaGameState, WhiteTeamKillScore);
}
