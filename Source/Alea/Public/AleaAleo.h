#pragma once

#include "CoreMinimal.h"
#include "AleaCharacter.h"
#include "AleaItemInPool.h"
#include "AleaAleo.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnTakeDamage);
DECLARE_DYNAMIC_DELEGATE(FOnDie);

UCLASS()
class ALEA_API AAleaAleo : public AAleaCharacter
{
	GENERATED_BODY()
	
public:
	AAleaAleo();

	FORCEINLINE uint8 GetNumber() const noexcept { return Number; }
	FORCEINLINE bool DoesMove() const noexcept { return bMoving; }

	UFUNCTION(Server, Reliable)
	void ServerAddKillCount();

	UFUNCTION(Server, Reliable)
	void ServerAddStickmanCount();

	UFUNCTION(Server, Reliable)
	void ServerAddExp(float Exp);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAddChips(int Chips);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerUseSkillPoints();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerConsumeMana(float ManaCost);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSetRCoolDownTime(float CoolDownTime, float MaxCoolDownTime = 0.f);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSetCanDeal(bool bOverlap);

	UFUNCTION(BlueprintCallable)
	class AAleaItemInPool* BuyItem(const TSubclassOf<AAleaItemInPool> ItemClass);

	UFUNCTION(BlueprintCallable)
	void SellItem(AAleaItemInPool* Item);

	UFUNCTION(BlueprintImplementableEvent)
	void LevelUp(bool bShowLevelUpButton);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	void ReqMove();
	void AckMove();

	UFUNCTION(Server, Reliable)
	void ServerRotateTo(const FRotator& Target);

	UFUNCTION(Server, Unreliable)
	void ServerMoveTo(const FVector& Goal);

	void FollowPath(class UNavigationPath* FollowPath);

	UFUNCTION(Server, Reliable)
	void ServerSetTarget(AAleaCharacter* Target);

	void ReqAutoAttack();
	void Detect();
	void AckAutoAttack();

	UFUNCTION(Server, Reliable)
	void ServerAttack(AAleaCharacter* Target, bool bMeleeAttack, float AttackDamage);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMeleeAttack(UParticleSystem* AttackEffect, USoundCue* AttackSound, UParticleSystem* HitEffect, const FVector& HitPoint);

	UFUNCTION(Server, Reliable)
	void ServerDie();

	UFUNCTION(Client, Reliable)
	void ClientDie();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLive();

	UFUNCTION(Server, Reliable)
	void ServerBuyItem(bool bNestable, uint8 ItemIndex);

	UFUNCTION(Server, Reliable)
	void ServerSellItem(bool bNestable, uint8 ItemIndex);

	UFUNCTION(Client, Reliable)
	void ClientLevelUp(bool bShowLevelUpButton);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUp();

	void Recall();

	UFUNCTION(Server, Reliable)
	void ServerRecall();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRecall();

	UFUNCTION(Server, Reliable)
	void ServerSetRecallEffect(bool bSet);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetRecallEffect(bool bSet);

	UFUNCTION(Client, Reliable)
	void ClientClearRecallTimer();

	void ToggleCameraLock();
	void CheckMousePosition(bool bXorY, bool& bLorT, bool& bRorB);
	void MoveCamera(bool bForR, bool bPorM);

	UPROPERTY(Replicated)
	uint8 Number;

	UPROPERTY(Replicated)
	uint8 bInitialize : 1;

	UPROPERTY(Replicated)
	uint8 bMoving : 1;

	UPROPERTY(Replicated)
	uint8 bRecall : 1;

	uint8 bPositioning : 1;

	uint8 bAutoAttacking : 1;

	uint8 bLockingCamera : 1;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 AvailableSkillPoints = 1;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bQ;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bW;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bE;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bR;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bOpenShop;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bCanDeal;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int KillCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int DeathCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int StickmanCount = 0;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int HoldingChips = 500;

	FVector DestLoc;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FTimerHandle RecallTimer;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FOnTakeDamage OnTakeDamage;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FOnDie OnDie;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<FAleaInventorySlot> Inventory;

	class USpringArmComponent* SpringArm;
	class UCameraComponent* LockedCamera;
	UCameraComponent* UnlockedCamera;
	class UParticleSystemComponent* Effect;
	UParticleSystemComponent* RecallEffect;
	class UAudioComponent* Audio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UWidgetComponent* LevelUpTextWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	class UDecalComponent* CursorHitPoint;

	class UDataTable* AleoTable;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDataTable* StatTable;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDataTable* SkillTable;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName Color;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 Level = 1;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxExperience;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Experience;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float HealthRegen;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxMana;

	UPROPERTY(Replicated, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Mana;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float ManaRegen;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float RespawnWaitTime;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float MaxRCoolDownTime;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float RCoolDownTime;
};
