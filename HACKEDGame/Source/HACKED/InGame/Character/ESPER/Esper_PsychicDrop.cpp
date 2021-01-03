// Fill out your copyright notice in the Description page of Project Settings.


#include "Esper_PsychicDrop.h"
#include "../ESPER/HACKED_ESPER.h"
#include "../HACKEDCharacter.h"
#include "../../AI/HACKED_AI.h"
#include "InGame/Stat/PlayerDamageEvent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/GameSession.h"
#include "InGame/Interface/HACKED_Enemy.h"
#include "InGame/HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"

// Sets default values
AEsper_PsychicDrop::AEsper_PsychicDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PS_PsychicDropParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PDParticleComponent"));
	PsychicDropSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PD Sphere Component"));
	PsychicDropProjectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("PD Projectile Component"));

	// °èÃþ ¼³Á¤ 

	RootComponent = PsychicDropSphere;
	PS_PsychicDropParticle->SetupAttachment(PsychicDropSphere);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ES_PSYCHICDROPTRAIL(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/M2/Esper_M2_shooting.Esper_M2_shooting"));
	if (ES_PSYCHICDROPTRAIL.Succeeded())
	{
		PS_PsychicDropParticle->SetTemplate(ES_PSYCHICDROPTRAIL.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ES_PSYCHICDROPEXPLOSION(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/M2/Esper_M2_Effect_Cast.Esper_M2_Effect_Cast"));
	PS_PsychicDropExplosion = ES_PSYCHICDROPEXPLOSION.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ES_PSYCHICDROPSTUN(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/M2/Esper_M2_Effect_Cast.Esper_M2_Effect_Cast"));
	PS_PsychicDropStun = ES_PSYCHICDROPSTUN.Object;

	PsychicDropProjectile->InitialSpeed = 1.0f;
	PsychicDropProjectile->ProjectileGravityScale = 0.0f;


	PsychicDropSphere->SetEnableGravity(false);
	PsychicDropSphere->SetCollisionProfileName(FName("ESPER_PD"));
	PsychicDropSphere->OnComponentHit.AddDynamic(this, &AEsper_PsychicDrop::OnComponentHit);

}

// Called when the game starts or when spawned
void AEsper_PsychicDrop::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(InGameManager);
	bigowner = InGameManager->GetCrusher();
}
// Called every frame
void AEsper_PsychicDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEsper_PsychicDrop::SetOwnerToEsper(AHACKED_ESPER* OwningActor)
{
	owner = OwningActor;
}

void AEsper_PsychicDrop::SetExplosionDamage(float damage)
{
	_pddamage = damage;
}

void AEsper_PsychicDrop::SetExplosionRange(float range)
{
	_pdrange = range;
}

void AEsper_PsychicDrop::SetSpeed(float speed)
{
	PsychicDropProjectile->Velocity = PsychicDropProjectile->Velocity * speed;
}

void AEsper_PsychicDrop::PsychicDropExplosion()
{
	TArray<AActor*> ignoreCharacter;
	ignoreCharacter.Add(bigowner);
	ignoreCharacter.Add(owner);
	ignoreCharacter.Add(this);
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		PS_PsychicDropExplosion,
		GetActorLocation(),
		GetActorRotation(), true);

	TArray<FHitResult> DetectResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		DetectResult,
		GetActorLocation(),
		GetActorLocation() + FVector(0.0f, 0.0f, 0.1f),
		FQuat::Identity,
	ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(_pdrange),
		Params);

	if (bResult)
	{
		for (auto Hit : DetectResult)
		{
			if (Hit.GetActor() && !ignoreCharacter.Contains(Hit.GetActor()))
			{
				AHACKED_AI* hackedAI = Cast<AHACKED_AI>(Hit.GetActor());
				if (!hackedAI) continue;

				FPlayerDamageEvent playerEvent;
				playerEvent.bCanChargeUlt = true;
				playerEvent.bCanExplosionEsperStack = true;
				playerEvent.bWithAim = true;
				playerEvent.bCanHitAnimation = true;
				Hit.GetActor()->TakeDamage(_pddamage, playerEvent, owner->GetController(), owner);

				hackedAI->LaunchCharacter((hackedAI->GetActorLocation() - GetActorLocation()).GetSafeNormal() * 400.0f, true, true);
			}
		}
		PlaySkillM2Sound();
	}

	Destroy();
}

void AEsper_PsychicDrop::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	PsychicDropProjectile->Velocity = FVector::ZeroVector;
}

