#include "AleaAleatoriumMode.h"
#include "AleaPlayerState.h"
#include "AleaStickman.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

AActor* AAleaAleatoriumMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AAleaPlayerState* PlayerState = Cast<AAleaPlayerState>(Player->PlayerState);

	if (GameState && PlayerState)
	{
		uint8 BlackPlayer = 0, WhitePlayer = 0;

		for (APlayerState* State : GameState->PlayerArray)
		{
			if (AAleaPlayerState* OtherPlayer = Cast<AAleaPlayerState>(State))
			{
				if (OtherPlayer->Team == TEXT("Black"))
				{
					++BlackPlayer;
				}
				else if (OtherPlayer->Team == TEXT("White"))
				{
					++WhitePlayer;
				}
			}
		}

		if (BlackPlayer > WhitePlayer)
		{
			PlayerState->Team = TEXT("White");
		}
		else
		{
			PlayerState->Team = TEXT("Black");
		}

		PlayerState->Number = PlayerIndex++;
	}

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	for (AActor* PlayerStart : PlayerStarts)
	{
		if (Cast<AAleaPlayerState>(Player->PlayerState)->Team == Cast<APlayerStart>(PlayerStart)->PlayerStartTag)
		{
			return PlayerStart;
		}
	}

	return nullptr;
}
