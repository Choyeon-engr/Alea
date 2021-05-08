#include "AleaStickmanAnim.h"
#include "AleaStickman.h"

void UAleaStickmanAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	Stickman = Cast<AAleaStickman>(TryGetPawnOwner());

	if (Stickman)
	{
		bDead = Stickman->IsDead();
		bMoving = Stickman->DoesMove();
		bAttacking = Stickman->DoesAttack();
		Game = Stickman->GetGame();
	}
}
