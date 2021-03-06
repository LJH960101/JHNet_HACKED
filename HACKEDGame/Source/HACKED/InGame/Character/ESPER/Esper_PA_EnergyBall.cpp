// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Esper_PA_EnergyBall.h"
#include "../ESPER/HACKED_ESPER.h"
#include "../HACKEDCharacter.h"
#include "../../AI/HACKED_AI.h"
#include "InGame/Stat/PlayerDamageEvent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/GameSession.h"
#include "InGame/Interface/HACKED_Enemy.h"

// Sets default values
AEsper_PA_EnergyBall::AEsper_PA_EnergyBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Esper Primary Attack Particle
	PAParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	primaryAttackDamageComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	// 계층 설정

	RootComponent = primaryAttackDamageComponent;
	PAParticle->SetupAttachment(primaryAttackDamageComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EP_PRIMARYATTACK(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/Primary/P_Primary_Projectile_v2.P_Primary_Projectile_v2"));
	if (EP_PRIMARYATTACK.Succeeded())
	{
		PAParticle->SetTemplate(EP_PRIMARYATTACK.Object);
	}
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> EP_PRIMARYATTACK(TEXT("ParticleSystem'/Game/Resources/Character/FX_Ver_2/FX2_Particle/Esper_Attack/P_Esper_Attack_Projectile.P_Esper_Attack_Projectile'"));
	//if (EP_PRIMARYATTACK.Succeeded())
	//{
	//	PAParticle->SetTemplate(EP_PRIMARYATTACK.Object);
	//}

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> EP_PRIMARYHITEP(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/Primary/P_PrimaryZap_Muzzle02.P_PrimaryZap_Muzzle02"));
	//PAHitParticle = EP_PRIMARYHITEP.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> EP_PRIMARYHITEP(TEXT("ParticleSystem'/Game/Resources/Character/FX_Ver_2/FX2_Particle/Esper_Attack/P_Esper_Attack_Shockwave.P_Esper_Attack_Shockwave'"));
	PAHitParticle = EP_PRIMARYHITEP.Object;

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> EP_PRIMARYSPLASH(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/Primary/P_PrimarySpledam.P_PrimarySpledam"));
	//PASplashParticle = EP_PRIMARYSPLASH.Object;
	
	ProjectileMovementComponent->InitialSpeed = 1.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	PAParticle->SetRelativeRotation(
		FQuat::MakeFromEuler(
			FVector(0.f, 0.f, 180.f)
		)
	);

	PAParticle->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	primaryAttackDamageComponent->SetEnableGravity(false);
	primaryAttackDamageComponent->SetCollisionProfileName(FName("ESPER_PA"));
	primaryAttackDamageComponent->OnComponentHit.AddDynamic(this, &AEsper_PA_EnergyBall::OnComponentHit);


}

// Called when the game starts or when spawned
void AEsper_PA_EnergyBall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEsper_PA_EnergyBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEsper_PA_EnergyBall::SetOwnerToEsper(AHACKED_ESPER * OwningActor)
{
	owner = OwningActor;
}

void AEsper_PA_EnergyBall::SetprimaryAttackDamage(float damage)
{
	_damage = damage;	
}

void AEsper_PA_EnergyBall::SetSPDamage(float spdamage)
{
	_spdamage = spdamage;
}

void AEsper_PA_EnergyBall::SetSPRange(float sprange)
{
	_sprange = sprange;
}

void AEsper_PA_EnergyBall::SetSpeed(float speed)
{
	ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity * speed;
}

USphereComponent * AEsper_PA_EnergyBall::GetSphere()
{
	return primaryAttackDamageComponent;
}

void AEsper_PA_EnergyBall::OnComponentHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (!OtherActor || OtherActor == owner) return;
	TArray<AActor*> ignoreCharacter;
	ignoreCharacter.Add(bigowner);
	ignoreCharacter.Add(owner);
	ignoreCharacter.Add(this);
	if (Cast<IHACKED_Enemy>(OtherActor)) 
	{
		FPlayerDamageEvent playerEvent;
		playerEvent.bCanEsperStack = true;
		playerEvent.bCanChargeUlt = true;
		playerEvent.bWithAim = true;
		OtherActor->TakeDamage(_damage + _spdamage, playerEvent, owner->GetController(), owner);
		ignoreCharacter.Add(OtherActor);
		//if (owner) 
		//{
		//	owner->SpSystemStartState();
		//}
	}

	TArray<FHitResult> DetectResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		DetectResult,
		Hit.Location,
		Hit.Location + FVector(0.0f, 0.0f, 0.1f),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(_sprange),
		Params);

	if (bResult)
	{
		for (auto HitResult : DetectResult)
		{
			if (HitResult.GetActor() && !ignoreCharacter.Contains(HitResult.GetActor()))
			{
				AHACKED_AI* hackedAI = Cast<AHACKED_AI>(HitResult.GetActor());
				if (!hackedAI) continue;

				FPlayerDamageEvent playerEvent;
				playerEvent.bCanEsperStack = true;
				playerEvent.bCanChargeUlt = true;
				playerEvent.bWithAim = true;
				HitResult.GetActor()->TakeDamage(_spdamage, playerEvent, owner->GetController(), owner);
			}
		}
	}

	//auto SplashScaleCG = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PASplashParticle, Hit.Location, GetActorRotation(), true);
	//SplashScaleCG->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
	auto HitParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PAHitParticle, Hit.Location, GetActorRotation(), true);
	HitParticle->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	OnExplosion();
	Destroy();
}






	


