#pragma once

#include "CoreMinimal.h"
#include "AleaActorInPool.h"
#include "AleaItemInPool.generated.h"

USTRUCT(BlueprintType)
struct FAleaInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	uint8 ItemIndex;

	UPROPERTY(BlueprintReadOnly)
	uint8 Quantity = 1;
};

UCLASS()
class ALEA_API AAleaItemInPool : public AAleaActorInPool
{
	GENERATED_BODY()

public:
	FORCEINLINE uint8 GetItemIndex() const noexcept { return ItemIndex; }
	FORCEINLINE bool IsNestable() const noexcept { return bNestable; }

	void SetOwningPlayer(class AAleaAleo* Aleo);

	UFUNCTION(BlueprintImplementableEvent)
	void Initialize();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Excute();

private:
	AAleaAleo* Owner;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	uint8 ItemIndex;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<AAleaItemInPool> ItemClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UTexture2D* Image;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int TotalCost;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int SoldFor;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bNestable;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bActive;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bConsumable;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bEnable = true;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float MaxCoolDownTime;
};
