#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AleaAleatoriumMode.generated.h"

UCLASS()
class ALEA_API AAleaAleatoriumMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	uint8 PlayerIndex;
};
