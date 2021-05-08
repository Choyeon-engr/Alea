#include "AleaAleo.h"
#include "AleaPlayerState.h"
#include "AleaGameState.h"
#include "AleaAleoRow.h"
#include "AleaAleoStatRow.h"
#include "AleaAleoSkillRow.h"
#include "AleaAleoSkillStatRow.h"
#include "AleaActorPool.h"
#include "AleaItemInPool.h"
#include "AleaAleatoriumMode.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AAleaAleo::AAleaAleo()
	: bLockingCamera(true)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 6000.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-60.f, 45.f, 0.f));

	LockedCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("LockedCamera"));
	LockedCamera->SetupAttachment(SpringArm);
	LockedCamera->bUsePawnControlRotation = false;

	UnlockedCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("UnlockedCamera"));
	UnlockedCamera->SetupAttachment(SpringArm);
	UnlockedCamera->bUsePawnControlRotation = false;

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->SetupAttachment(RootComponent);

	RecallEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RecallEffect"));
	RecallEffect->SetupAttachment(RootComponent);
	RecallEffect->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	RecallEffect->SetRelativeScale3D(FVector(1.5f));

	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(RootComponent);

	LevelUpTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LevelUpText"));
	LevelUpTextWidget->SetupAttachment(RootComponent);

	CursorHitPoint = CreateDefaultSubobject<UDecalComponent>(TEXT("CursorHitPoint"));
	CursorHitPoint->SetupAttachment(RootComponent);
	CursorHitPoint->SetVisibility(false);

	ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/DataTable/Aleo/AleoTable"));
	AleoTable = DataTable.Object;
}

void AAleaAleo::ServerAddKillCount_Implementation()
{
	++KillCount;
}

void AAleaAleo::ServerAddStickmanCount_Implementation()
{
	++StickmanCount;
}

void AAleaAleo::ServerAddExp_Implementation(float Exp)
{
	Experience += Exp;

	if (Experience >= MaxExperience)
	{
		++Level;
		++AvailableSkillPoints;

		Experience = 0.f;
		MaxExperience = 280.f + (Level - 1.f) * 100.f;

		float NewMaxHealth = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetHealth();
		Health += NewMaxHealth - MaxHealth;
		MaxHealth = NewMaxHealth;

		float NewMaxMana = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetMana();
		Mana += NewMaxMana - MaxMana;
		MaxMana = NewMaxMana;

		ClientLevelUp(AvailableSkillPoints == 1);
		MulticastLevelUp();
	}
}

void AAleaAleo::ServerAddChips_Implementation(int Chips)
{
	HoldingChips += Chips;
}                                                                                                                                                  

void AAleaAleo::ServerUseSkillPoints_Implementation()
{
	--AvailableSkillPoints;
}

void AAleaAleo::ServerConsumeMana_Implementation(float ManaCost)
{
	Mana -= ManaCost;
}

void AAleaAleo::ServerSetRCoolDownTime_Implementation(float CoolDownTime, float MaxCoolDownTime)
{
	if (MaxCoolDownTime)
	{
		MaxRCoolDownTime = MaxCoolDownTime;
	}
	
	RCoolDownTime = CoolDownTime > 0.f ? CoolDownTime : 0.f;
}

void AAleaAleo::ServerSetCanDeal_Implementation(bool bOverlap)
{
	bCanDeal = bOverlap;
}

AAleaItemInPool* AAleaAleo::BuyItem(const TSubclassOf<AAleaItemInPool> ItemClass)
{
	AAleaItemInPool* Item = Cast<AAleaItemInPool>(ActorPool->Spawn(ItemClass, GetActorTransform()));
	Item->Initialize();
	Item->SetOwningPlayer(this);
	ServerBuyItem(Item->IsNestable(), Item->GetItemIndex());

	return Item;
}

void AAleaAleo::SellItem(AAleaItemInPool* Item)
{
	ServerSellItem(Item->IsNestable(), Item->GetItemIndex());
	Item->SetOwningPlayer(nullptr);
}

void AAleaAleo::BeginPlay()
{
	AAleaCharacter::BeginPlay();

	UnlockedCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	StatTable = AleoTable->FindRow<FAleaAleoRow>(Color, TEXT(""))->GetAleoStatTable();
	SkillTable = AleoTable->FindRow<FAleaAleoRow>(Color, TEXT(""))->GetAleoSkillTable();

	MaxExperience = 280.f + (Level - 1.f) * 100.f;

	MaxHealth = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetHealth();
	Health = MaxHealth;
	HealthRegen = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetHealthRegen();

	MaxMana = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetMana();
	Mana = MaxMana;
	ManaRegen = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetManaRegen();

	GetCharacterMovement()->MaxWalkSpeed = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetMovementSpeed();

	if (HasAuthority())
	{
		FTimerHandle RegenTimer;
		GetWorldTimerManager().SetTimer(RegenTimer, FTimerDelegate::CreateLambda([this]() -> void
			{
				if (!bDead)
				{
					if (bCanDeal)
					{
						if (Health < MaxHealth)
						{
							Health = Health + HealthRegen * 20.f > MaxHealth ? MaxHealth : Health + HealthRegen * 20.f;
						}

						if (Mana < MaxMana)
						{
							Mana = Mana + ManaRegen * 20.f > MaxMana ? MaxMana : Mana + ManaRegen * 20.f;
						}
					}
					else
					{
						HealthRegen = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetHealthRegen();
						ManaRegen = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetManaRegen();

						if (Health < MaxHealth)
						{
							Health = Health + HealthRegen > MaxHealth ? MaxHealth : Health + HealthRegen;
						}

						if (Mana < MaxMana)
						{
							Mana = Mana + ManaRegen > MaxMana ? MaxMana : Mana + ManaRegen;
						}
					}
				}
			}
		), 0.5f, true, 0.f);

		FTimerHandle ChipObtainTimer;
		GetWorldTimerManager().SetTimer(ChipObtainTimer, FTimerDelegate::CreateLambda([this]() -> void
			{
				HoldingChips += 1;
			}
		), 1.f, true, 90.f);
	}
}

void AAleaAleo::Tick(float DeltaSeconds)
{
	AAleaCharacter::Tick(DeltaSeconds);
	
	bMoving = GetVelocity().Size() > 0.f;

	if (!bInitialize && HasAuthority())
	{
		Team = GetPlayerState<AAleaPlayerState>()->Team;
		Number = GetPlayerState<AAleaPlayerState>()->Number;
		bInitialize = true;
	}

	if (bDead)
	{
		RespawnWaitTime -= DeltaSeconds;

		if (RespawnWaitTime <= 0.f)
		{
			bDead = false;
			Health = MaxHealth;
			Mana = MaxMana;

			MulticastLive();
		}
	}
	else
	{
		if (bPositioning || DestLoc != FVector::ZeroVector)
		{
			AckMove();
		}

		if (bAutoAttacking)
		{
			Detect();
		}

		if (bRecall)
		{
			RecallEffect->SetRelativeScale3D(FMath::VInterpTo(RecallEffect->GetRelativeScale3D(), FVector(0.5f), DeltaSeconds, 0.1f));
		}
	}

	if (bLockingCamera)
	{
		UnlockedCamera->SetWorldLocation(LockedCamera->GetComponentLocation());
	}
	else
	{
		bool bLorT, bRorB;

		CheckMousePosition(true, bLorT, bRorB);

		if (bLorT)
		{
			MoveCamera(false, false);
		}
		else if (bRorB)
		{
			MoveCamera(false, true);
		}

		CheckMousePosition(false, bLorT, bRorB);

		if (bLorT)
		{
			MoveCamera(true, true);
		}
		else if (bRorB)
		{
			MoveCamera(true, false);
		}
	}

	FHitResult HitResult;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	CursorHitPoint->SetWorldLocationAndRotation(HitResult.Location, HitResult.ImpactNormal.Rotation());
}

void AAleaAleo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Move"), IE_Pressed, this, &AAleaAleo::ReqMove);
	PlayerInputComponent->BindAction(TEXT("Move"), IE_Released, this, &AAleaAleo::ReqMove);

	PlayerInputComponent->BindAction(TEXT("AutoAttack"), IE_Pressed, this, &AAleaAleo::ReqAutoAttack);

	PlayerInputComponent->BindAction(TEXT("ToggleCameraLock"), IE_Pressed, this, &AAleaAleo::ToggleCameraLock);

	PlayerInputComponent->BindAction(TEXT("Recall"), IE_Pressed, this, &AAleaAleo::Recall);
}

void AAleaAleo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	AAleaCharacter::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAleaAleo, Number);
	DOREPLIFETIME(AAleaAleo, bInitialize);
	DOREPLIFETIME(AAleaAleo, bMoving);
	DOREPLIFETIME(AAleaAleo, bRecall);
	DOREPLIFETIME(AAleaAleo, AvailableSkillPoints);
	DOREPLIFETIME(AAleaAleo, bR);
	DOREPLIFETIME(AAleaAleo, bCanDeal);
	DOREPLIFETIME(AAleaAleo, KillCount);
	DOREPLIFETIME(AAleaAleo, DeathCount);
	DOREPLIFETIME(AAleaAleo, StickmanCount);
	DOREPLIFETIME(AAleaAleo, HoldingChips);
	DOREPLIFETIME(AAleaAleo, Inventory);
	DOREPLIFETIME(AAleaAleo, Level);
	DOREPLIFETIME(AAleaAleo, MaxExperience);
	DOREPLIFETIME(AAleaAleo, Experience);
	DOREPLIFETIME(AAleaAleo, HealthRegen);
	DOREPLIFETIME(AAleaAleo, MaxMana);
	DOREPLIFETIME(AAleaAleo, Mana);
	DOREPLIFETIME(AAleaAleo, ManaRegen);
	DOREPLIFETIME(AAleaAleo, RespawnWaitTime);
	DOREPLIFETIME(AAleaAleo, RCoolDownTime);
	DOREPLIFETIME(AAleaAleo, MaxRCoolDownTime);
}

float AAleaAleo::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = AAleaCharacter::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health -= FinalDamage;

	if (!bDead && Health <= 0.f)
	{
		ServerDie();
		
		if (AAleaAleo* Enemy = Cast<AAleaAleo>(DamageCauser))
		{
			Enemy->ServerAddExp(90.f + Level * 50.f);
			Enemy->ServerAddKillCount();
			Enemy->ServerAddChips(300);

			MulticastPlayDroppedChipsText(Enemy->GetNumber());

			if (Team == TEXT("Black"))
			{
				GetWorld()->GetGameState<AAleaGameState>()->AddWhiteTeamKillScore();
			}
			else if (Team == TEXT("White"))
			{
				GetWorld()->GetGameState<AAleaGameState>()->AddBlackTeamKillScore();
			}
		}
	}
	else if (!HasAuthority() && UGameplayStatics::GetPlayerController(this, 0) == GetController())
	{
		OnTakeDamage.Execute();
	}

	ClientClearRecallTimer();

	return FinalDamage;
}

void AAleaAleo::ReqMove()
{
	if (!bDead && !bOpenShop)
	{
		bPositioning = !bPositioning;

		FHitResult HitResult;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		if (!bPositioning)
		{
			DestLoc = HitResult.ImpactPoint;
		}
		else
		{
			DestLoc = FVector::ZeroVector;

			CursorHitPoint->SetVisibility(true);

			FTimerHandle HideCursorHitPointTimer;
			GetWorldTimerManager().SetTimer(HideCursorHitPointTimer, FTimerDelegate::CreateLambda([this, &HideCursorHitPointTimer]() -> void
				{
					CursorHitPoint->SetVisibility(false);
				}
			), 1.f, false, 0.f);
		}

		GetWorldTimerManager().ClearTimer(RecallTimer);
		ServerSetRecallEffect(false);
	}
}

void AAleaAleo::AckMove()
{
	FVector Point;

	if (bPositioning)
	{
		FHitResult HitResult;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		Point = HitResult.ImpactPoint;
	}
	else
	{
		Point = DestLoc;
	}

	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetActorLocation(), Point))
	{
		if (NavPath->PathPoints.Num() > 1)
		{
			FollowPath(NavPath);

			bAutoAttacking = false;
			TrgtAutoAttacked = nullptr;
			ServerSetTarget(TrgtAutoAttacked);
		}
	}
}

void AAleaAleo::ServerRotateTo_Implementation(const FRotator& Target)
{
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), Target, GetWorld()->GetDeltaSeconds(), 640.f));
}

void AAleaAleo::ServerMoveTo_Implementation(const FVector& Goal)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Goal);
}

void AAleaAleo::FollowPath(UNavigationPath* FollowPath)
{
	FVector ToPoint(FollowPath->PathPoints[1] - GetActorLocation());
	ToPoint.Z = 0.f;

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), ToPoint.Rotation(), GetWorld()->GetDeltaSeconds(), 640.f));

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FollowPath->PathPoints[1]);
	ServerMoveTo(FollowPath->PathPoints[1]);

	if (!bMoving && FollowPath->PathPoints.Num() > 2)
	{
		FVector ToNextPoint(FollowPath->PathPoints[2] - GetActorLocation());
		ToNextPoint.Z = 0.f;

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), ToNextPoint.Rotation(), GetWorld()->GetDeltaSeconds(), 640.f));

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FollowPath->PathPoints[2]);
		ServerMoveTo(FollowPath->PathPoints[2]);
	}
}

void AAleaAleo::ServerSetTarget_Implementation(AAleaCharacter* Target)
{
	TrgtAutoAttacked = Target;
}

void AAleaAleo::ReqAutoAttack()
{
	if (!bDead)
	{
		const ECollisionChannel& AleaTrace = ECC_GameTraceChannel1;
		FHitResult HitResult;

		if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHitResultUnderCursor(AleaTrace, false, HitResult))
		{
			if (Cast<AAleaCharacter>(HitResult.Actor)->Team != Team)
			{
				bAutoAttacking = true;
				TrgtAutoAttacked = Cast<AAleaCharacter>(HitResult.Actor);
				ServerSetTarget(TrgtAutoAttacked);

				DestLoc = FVector::ZeroVector;
				GetWorldTimerManager().ClearTimer(RecallTimer);
				ServerSetRecallEffect(false);
			}
		}
	}
}

void AAleaAleo::Detect()
{
	if (TrgtAutoAttacked && TrgtAutoAttacked != this && !TrgtAutoAttacked->IsDead())
	{
		FVector ToEnemy = TrgtAutoAttacked->GetActorLocation() - GetActorLocation();
		ToEnemy.Z = 0.f;

		float Range = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->GetAleoSkillStatTable()
			->FindRow<FAleaAleoSkillStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetRange();

		if (ToEnemy.Size() <= Range)
		{
			AckAutoAttack();
		}
		else
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), TrgtAutoAttacked))
			{
				if (NavPath->PathPoints.Num() > 1)
				{
					FollowPath(NavPath);
				}
			}
		}
	}
	else
	{
		bAutoAttacking = false;
		TrgtAutoAttacked = nullptr;
		ServerSetTarget(TrgtAutoAttacked);
	}
}

void AAleaAleo::AckAutoAttack()
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
	ServerMoveTo(GetActorLocation());

	FVector ToEnemy(TrgtAutoAttacked->GetActorLocation() - GetActorLocation());
	ToEnemy.Z = 0.f;

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), ToEnemy.Rotation(), GetWorld()->GetDeltaSeconds(), 640.f));
	ServerRotateTo(ToEnemy.Rotation());

	if (AutoAttackCoolDownTime <= 0.f)
	{
		bool bMeleeAttack = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->IsMeleeAttack();
		float AttackDamage = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->GetAleoSkillStatTable()
			->FindRow<FAleaAleoSkillStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetAttackDamage();

		ServerAttack(TrgtAutoAttacked, bMeleeAttack, AttackDamage);

		AutoAttackCoolDownTime = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->GetAleoSkillStatTable()
			->FindRow<FAleaAleoSkillStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetCoolDownTime();
	}
}

void AAleaAleo::ServerAttack_Implementation(AAleaCharacter* Target, bool bMeleeAttack, float AttackDamage)
{
	UParticleSystem* AttackEffect = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->GetAttackEffect();
	USoundCue* AttackSound = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->GetAttackSound();
	UParticleSystem* HitEffect = SkillTable->FindRow<FAleaAleoSkillRow>(TEXT("AutoAttack"), TEXT(""))->GetHitEffect();

	if (bMeleeAttack)
	{
		MulticastMeleeAttack(AttackEffect, AttackSound, HitEffect, Target->GetMesh()->GetSocketLocation(TEXT("HitPoint")));
		UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this, nullptr);
	}
	else
	{
		MulticastRangedAttack(AttackEffect, AttackSound, HitEffect, this, Target, AttackDamage);
	}
}

void AAleaAleo::MulticastMeleeAttack_Implementation(UParticleSystem* AttackEffect, USoundCue* AttackSound, UParticleSystem* HitEffect, const FVector& HitPoint)
{
	Effect->SetTemplate(AttackEffect);
	Audio->SetSound(AttackSound);
	Audio->Play();

	UGameplayStatics::SpawnEmitterAtLocation(this, HitEffect, HitPoint, GetActorRotation());
}

void AAleaAleo::ServerDie_Implementation()
{
	bDead = true;
	TrgtAutoAttacked = nullptr;

	++DeathCount;
	Mana = 0.f;
	RespawnWaitTime = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetRespawnWaitTime();

	ClientDie();
	MulticastDie();
}

void AAleaAleo::ClientDie_Implementation()
{
	OnDie.Execute();

	bPositioning = false;
	bAutoAttacking = false;
	DestLoc = FVector::ZeroVector;
}

void AAleaAleo::MulticastLive_Implementation()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), OutActors);

	for (AActor* OutActor : OutActors)
	{
		if (Team == Cast<APlayerStart>(OutActor)->PlayerStartTag)
		{
			TeleportTo(OutActor->GetActorLocation(), OutActor->GetActorRotation());
			break;
		}
	}

	HealthBarWidget->SetVisibility(true);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PhysicsActor"));
	GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
}

void AAleaAleo::ServerBuyItem_Implementation(bool bNestable, uint8 ItemIndex)
{
	if (bNestable)
	{
		for (auto& InventorySlot : Inventory)
		{
			if (InventorySlot.ItemIndex == ItemIndex)
			{
				++InventorySlot.Quantity;
				return;
			}
		}
	}

	FAleaInventorySlot InventorySlot;
	InventorySlot.ItemIndex = ItemIndex;
	Inventory.Add(InventorySlot);
}

void AAleaAleo::ServerSellItem_Implementation(bool bNestable, uint8 ItemIndex)
{
	if (bNestable)
	{
		for (auto& InventorySlot : Inventory)
		{
			if (InventorySlot.ItemIndex == ItemIndex && InventorySlot.Quantity > 1)
			{
				--InventorySlot.Quantity;
				return;
			}
		}
	}
	
	for (int Idx = 0; Idx < Inventory.Num(); ++Idx)
	{
		if (Inventory[Idx].ItemIndex == ItemIndex)
		{
			Inventory.RemoveAt(Idx);
			return;
		}
	}
}

void AAleaAleo::ClientLevelUp_Implementation(bool bShowLevelUpButton)
{
	LevelUp(bShowLevelUpButton);
}

void AAleaAleo::MulticastLevelUp_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = StatTable->FindRow<FAleaAleoStatRow>(*(FString::FormatAsNumber(Level)), TEXT(""))->GetMovementSpeed();

	Effect->SetTemplate(AleoTable->FindRow<FAleaAleoRow>(Color, TEXT(""))->GetLevelUpEffect());
	Audio->SetSound(AleoTable->FindRow<FAleaAleoRow>(Color, TEXT(""))->GetLevelUpSound());
	Audio->Play();
}

void AAleaAleo::Recall()
{
	if (!bDead)
	{
		ServerSetRecallEffect(false);
		GetWorldTimerManager().ClearTimer(RecallTimer);

		bAutoAttacking = false;
		TrgtAutoAttacked = nullptr;
		ServerSetTarget(TrgtAutoAttacked);

		bPositioning = false;
		DestLoc = FVector::ZeroVector;

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
		ServerMoveTo(GetActorLocation());

		ServerSetRecallEffect(true);

		GetWorldTimerManager().SetTimer(RecallTimer, FTimerDelegate::CreateLambda([this]() -> void
			{
				ServerSetRecallEffect(false);
				ServerRecall();
				GetWorldTimerManager().ClearTimer(RecallTimer);
			}
		), 8.f, false);
	}
}

void AAleaAleo::ServerRecall_Implementation()
{
	MulticastRecall();
}

void AAleaAleo::MulticastRecall_Implementation()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), OutActors);

	for (AActor* OutActor : OutActors)
	{
		if (Team == Cast<APlayerStart>(OutActor)->PlayerStartTag)
		{
			TeleportTo(OutActor->GetActorLocation(), OutActor->GetActorRotation());
			break;
		}
	}
}

void AAleaAleo::ServerSetRecallEffect_Implementation(bool bSet)
{
	bRecall = bSet;
	MulticastSetRecallEffect(bSet);
}

void AAleaAleo::MulticastSetRecallEffect_Implementation(bool bSet)
{
	RecallEffect->SetTemplate(bSet ? AleoTable->FindRow<FAleaAleoRow>(Color, TEXT(""))->GetRecallEffect() : nullptr);
	RecallEffect->SetRelativeScale3D(FVector(1.5f));
}

void AAleaAleo::ClientClearRecallTimer_Implementation()
{
	GetWorldTimerManager().ClearTimer(RecallTimer);
	ServerSetRecallEffect(false);
}

void AAleaAleo::ToggleCameraLock()
{
	bLockingCamera = !bLockingCamera;

	if (bLockingCamera)
	{
		LockedCamera->SetActive(true);
		UnlockedCamera->SetActive(false);
	}
	else
	{
		LockedCamera->SetActive(false);
		UnlockedCamera->SetActive(true);
	}
}

void AAleaAleo::CheckMousePosition(bool bXorY, bool& bLorT, bool& bRorB)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	float LocationX, LocationY;
	PlayerController->GetMousePosition(LocationX, LocationY);
	float Mouse = bXorY ? LocationX : LocationY;

	int32 SizeX, SizeY;
	PlayerController->GetViewportSize(SizeX, SizeY);
	float Viewport = bXorY ? SizeX : SizeY;

	bLorT = Mouse <= Viewport / 9.6f;
	bRorB = Mouse >= Viewport - Viewport / 9.6f;
}

void AAleaAleo::MoveCamera(bool bForR, bool bPorN)
{
	float Direction = bForR ? 100.f : 0.f;
	float DirectionR = !bForR ? 100.f : 0.f;
	float Axis = bPorN ? 1.f : -1.f;

	FVector DstCameraLocation(UnlockedCamera->GetComponentLocation());
	DstCameraLocation.X += Direction * Axis;
	DstCameraLocation.Y += DirectionR * Axis;

	UnlockedCamera->SetWorldLocation(FMath::VInterpTo(UnlockedCamera->GetComponentLocation(), DstCameraLocation, GetWorld()->GetDeltaSeconds(), 20.f));
}
