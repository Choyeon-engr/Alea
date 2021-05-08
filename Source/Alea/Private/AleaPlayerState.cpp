#include "AleaPlayerState.h"

#include "Net/UnrealNetwork.h"

void AAleaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAleaPlayerState, Team);
	DOREPLIFETIME(AAleaPlayerState, Number);
}
