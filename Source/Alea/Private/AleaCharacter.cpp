#include "AleaCharacter.h"
#include "AleaActorPool.h"
#include "AleaGameInstance.h"
#include "AleaProjectileInPool.h"
#include "AleaAleo.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAleaCharacter::AAleaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("PhysicsActor"));
	GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(RootComponent);

	DamageTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageText"));
	DamageTextWidget->SetupAttachment(RootComponent);

	DroppedChipsTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DroppedChipsText"));
	DroppedChipsTextWidget->SetupAttachment(RootComponent);
}

void AAleaCharacter::BeginPlay()
{
	Super::BeginPlay();

	ActorPool = CastChecked<UAleaGameInstance>(GetGameInstance())->GetActorPool();
}

void AAleaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AutoAttackCoolDownTime -= DeltaSeconds;
}

void AAleaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAleaCharacter, Team);
	DOREPLIFETIME(AAleaCharacter, bDead);
	DOREPLIFETIME(AAleaCharacter, MaxHealth);
	DOREPLIFETIME(AAleaCharacter, Health);
	DOREPLIFETIME(AAleaCharacter, TrgtAutoAttacked);
}

float AAleaCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (AAleaAleo* Aleo = Cast<AAleaAleo>(DamageCauser))
	{
		ServerPlayDamageText(FinalDamage, Aleo->GetNumber());
	}

	return FinalDamage;
}

void AAleaCharacter::MulticastRangedAttack_Implementation(UParticleSystem* AttackEffect, class USoundCue* AttackSound, UParticleSystem* Hit, class AAleaCharacter* Spwnr, AAleaCharacter* Trgt, float Damage)
{
	AAleaProjectileInPool* Projectile = Cast<AAleaProjectileInPool>(ActorPool->Spawn(AAleaProjectileInPool::StaticClass(), GetActorTransform()));
	Projectile->Initialize(AttackEffect, AttackSound, Hit, Spwnr, Trgt, Damage);
}

void AAleaCharacter::MulticastDie_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Die"));
	GetMesh()->SetCollisionProfileName(TEXT("Die"));

	HealthBarWidget->SetVisibility(false);
}

void AAleaCharacter::ServerPlayDamageText_Implementation(float FinalDamage, uint8 PlayerNumber)
{
	MulticastPlayDamageText(FinalDamage, PlayerNumber);
}

void AAleaCharacter::MulticastPlayDamageText_Implementation(float FinalDamage, uint8 PlayerNumber)
{
	if (PlayerNumber == Cast<AAleaAleo>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetNumber())
	{
		PlayDamageText(FinalDamage);
	}
}

void AAleaCharacter::MulticastPlayDroppedChipsText_Implementation(uint8 PlayerNumber)
{
	if (PlayerNumber == Cast<AAleaAleo>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->GetNumber())
	{
		PlayDroppedChipsText();
	}
}
