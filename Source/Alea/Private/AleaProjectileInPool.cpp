#include "AleaProjectileInPool.h"
#include "AleaCharacter.h"
#include "AleaAleo.h"
#include "AleaStickman.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AAleaProjectileInPool::AAleaProjectileInPool()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	SphereCollision->InitSphereRadius(5.f);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AAleaProjectileInPool::BeginOverlap);
	RootComponent = SphereCollision;

	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));
	Effect->SetupAttachment(RootComponent);
	
	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	Audio->SetupAttachment(RootComponent);
}

void AAleaProjectileInPool::Initialize(UParticleSystem* AttackEffect, class USoundCue* AttackSound, UParticleSystem* Hit, class AAleaCharacter* Spwnr, AAleaCharacter* Trgt, float Damage)
{
	Effect->SetTemplate(AttackEffect);

	if (AttackSound)
	{
		Audio->SetSound(AttackSound);
		Audio->Play();
	}

	HitEffect = Hit;
	Spawner = Spwnr;
	Target = Trgt;
	AttackDamage = Damage;
}

void AAleaProjectileInPool::BeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bActive && Target == OtherActor)
	{
		bActive = false;
		UGameplayStatics::ApplyDamage(Target, AttackDamage, Spawner->GetController(), Spawner, nullptr);
		UGameplayStatics::SpawnEmitterAtLocation(this, HitEffect, Target->GetMesh()->GetSocketLocation(TEXT("HitPoint")), GetActorRotation());
		AAleaProjectileInPool::Release();
	}
}

void AAleaProjectileInPool::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Target && IsValid(Target->GetRootComponent()) && !Target->IsDead())
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), Target->GetActorLocation(), DeltaSeconds, 5.f));
	}
	else
	{
		AAleaProjectileInPool::Release();
	}
}

void AAleaProjectileInPool::Release()
{
	Effect->SetTemplate(nullptr);

	Audio->Stop();
	Audio->SetSound(nullptr);

	HitEffect = nullptr;
	Spawner = nullptr;
	Target = nullptr;
	AttackDamage = 0.f;

	AAleaActorInPool::Release();
}
