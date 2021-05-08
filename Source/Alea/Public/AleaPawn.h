#pragma once

#include "CoreMinimal.h"
#include "AleaCharacter.h"
#include "AleaPawn.generated.h"

UCLASS()
class ALEA_API AAleaPawn : public AAleaCharacter
{
	GENERATED_BODY()
	
public:
	AAleaPawn();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	void Detect();
	void FindNearestEnemy(const TArray<AAleaCharacter*>& Enemies);
	void Attack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UStaticMeshComponent* StaticMesh;

	uint8 AttackingAleoCnt;

	class UDataTable* TeamTable;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDataTable* PawnTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName Position;
};
