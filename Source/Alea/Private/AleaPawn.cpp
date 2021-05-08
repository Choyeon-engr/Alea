#include "AleaPawn.h"
#include "AleaTeamRow.h"
#include "AleaPawnRow.h"
#include "AleaActorPool.h"
#include "AleaProjectileInPool.h"
#include "AleaAleo.h"
#include "AleaStickman.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

AAleaPawn::AAleaPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetCanEverAffectNavigation(false);

	ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/DataTable/Team/TeamTable"));
	TeamTable = DataTable.Object;
}

void AAleaPawn::BeginPlay()
{
	AAleaCharacter::BeginPlay();

	PawnTable = TeamTable->FindRow<FAleaTeamRow>(Team, TEXT(""))->GetPawnTable();

	MaxHealth = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetHealth();
	Health = MaxHealth;
}

void AAleaPawn::Tick(float DeltaSeconds)
{
	AAleaCharacter::Tick(DeltaSeconds);

	Detect();

	if (TrgtAutoAttacked)
	{
		Attack();
	}
	else
	{
		AttackingAleoCnt = 0;
	}
}

float AAleaPawn::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = AAleaCharacter::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	Health -= FinalDamage;

	if (!bDead && Health <= 0.f)
	{
		MulticastDestroyPawn();
	}

	return FinalDamage;
}

void AAleaPawn::Detect()
{
	TArray<FOverlapResult> OutOverlaps;
	const ECollisionChannel& AleaTrace = ECC_GameTraceChannel1;
	float DetectRange = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetRange();
	FCollisionQueryParams Params(NAME_None, false, this);

	if (GetWorld()->OverlapMultiByChannel(OutOverlaps, GetActorLocation(), FQuat::Identity, AleaTrace, FCollisionShape::MakeSphere(DetectRange), Params))
	{
		TArray<AAleaCharacter*> Enemies, AleosTargetedAleo, AleosNontargetedAleo, Cards, Dices;

		for (FOverlapResult OutOverlap : OutOverlaps)
		{
			if (AAleaCharacter* Target = Cast<AAleaCharacter>(OutOverlap.Actor))
			{
				if (Team != Target->Team)
				{
					Enemies.Add(Target);
				}
			}
		}

		if (!Enemies.Num())
		{
			TrgtAutoAttacked = nullptr;
			return;
		}

		for (AAleaCharacter* Enemy : Enemies)
		{
			if (Cast<AAleaAleo>(Enemy))
			{
				if (Cast<AAleaAleo>(Enemy->GetTarget()))
				{
					AleosTargetedAleo.Add(Enemy);
				}
				else
				{
					AleosNontargetedAleo.Add(Enemy);
				}
			}
			else if (AAleaStickman* Stickman = Cast<AAleaStickman>(Enemy))
			{
				if (Stickman->GetGame() == TEXT("Card"))
				{
					Cards.Add(Stickman);
				}
				else if (Stickman->GetGame() == TEXT("Dice"))
				{
					Dices.Add(Stickman);
				}
			}
		}

		if (AleosTargetedAleo.Num())
		{
			FindNearestEnemy(AleosTargetedAleo);
		}
		else if (Dices.Num())
		{
			FindNearestEnemy(Dices);
		}
		else if (Cards.Num())
		{
			FindNearestEnemy(Cards);
		}
		else if (AleosNontargetedAleo.Num())
		{
			FindNearestEnemy(AleosNontargetedAleo);
		}
	}
	else
	{
		TrgtAutoAttacked = nullptr;
	}
}

void AAleaPawn::FindNearestEnemy(const TArray<AAleaCharacter*>& Enemies)
{
	float ShortestRange = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetRange();

	for (AAleaCharacter* Enemy : Enemies)
	{
		if (GetDistanceTo(Enemy) <= ShortestRange)
		{
			ShortestRange = GetDistanceTo(Enemy);
			TrgtAutoAttacked = Enemy;
		}
	}
}

void AAleaPawn::Attack()
{
	if (AutoAttackCoolDownTime <= 0.f)
	{
		float AttackDamage = 0.f;

		if (Cast<AAleaAleo>(TrgtAutoAttacked))
		{
			AttackDamage = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetBaseDamage();
			AttackDamage += AttackDamage * PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetAleoDmgRate() * AttackingAleoCnt;

			if (AttackingAleoCnt < 3)
			{
				++AttackingAleoCnt;
			}
		}
		else
		{
			AttackingAleoCnt = 0;
			AAleaStickman* Enemy = Cast<AAleaStickman>(TrgtAutoAttacked);

			if (Enemy->GetGame() == TEXT("Card"))
			{
				AttackDamage = Enemy->GetMaxHealth() * PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetCardDmgRate();
			}
			else if (Enemy->GetGame() == TEXT("Dice"))
			{
				AttackDamage = Enemy->GetMaxHealth() * PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetDiceDmgRate();
			}
		}

		UParticleSystem* AttackEffect = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetAttackEffect();
		USoundCue* AttackSound = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetAttackSound();
		UParticleSystem* HitEffect = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetHitEffect();

		MulticastRangedAttack(AttackEffect, AttackSound, HitEffect, this, TrgtAutoAttacked, AttackDamage);

		AutoAttackCoolDownTime = PawnTable->FindRow<FAleaPawnRow>(Position, TEXT(""))->GetCoolDownTime();
	}
}

void AAleaPawn::MulticastDestroyPawn_Implementation()
{
	GetWorld()->DestroyActor(this);
}
