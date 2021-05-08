#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AleaStickmanAnim.generated.h"

UCLASS()
class ALEA_API UAleaStickmanAnim : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	class AAleaStickman* Stickman;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bDead;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bMoving;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bAttacking;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName Game;
};
