// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotonShot.h"
#include "InGame/Character/CRUSHER/Barrier.h"
#include "GameFramework/DamageType.h"

// Sets default values
APhotonShot::APhotonShot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PC_PhotonShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PhotonShotPC"));
	PhotonShotSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PhotonShotSphere"));
	PhotonShotProjectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("PhotonShotProjectile"));
	
	// °èÃþ ¼³Á¤

	RootComponent = PhotonShotSphere;
	PC_PhotonShotParticle->SetupAttachment(PhotonShotSphere);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PHOTONSHOTPROJECTILE(TEXT("/Game/HACKED_NamedAI/PhotonShot.PhotonShot"));
	if (PHOTONSHOTPROJECTILE.Succeeded())
	{
		PC_PhotonShotParticle->SetTemplate(PHOTONSHOTPROJECTILE.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PHOTONSHOTEXPLOSION(TEXT("/Game/HACKED_NamedAI/PhotonShotExplosion.PhotonShotExplosion"));
	PS_PhotonShotExplosion = PHOTONSHOTEXPLOSION.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_PHOTONHIT(TEXT("/Game/Sound/Ai_Named_Laser/Ai_Named_Laser_Hit/Named_PhotonHit_Cue.Named_PhotonHit_Cue"));
	SB_PhotonHit = SB_PHOTONHIT.Object;

	PhotonShotProjectile->InitialSpeed = 1.0f;
	PhotonShotProjectile->ProjectileGravityScale = 0.0f;

	PhotonShotSphere->SetEnableGravity(false);
	PhotonShotSphere->SetCollisionProfileName(FName("AIShooting"));
	PhotonShotSphere->OnComponentHit.AddDynamic(this, &APhotonShot::OnComponentHit);


}

// Called when the game starts or when spawned
void APhotonShot::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APhotonShot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APhotonShot::SetOwnerToMN(AMilitaryNamedAI* OwningActor)
{
	owner = OwningActor;
}

void APhotonShot::SetPhotonShotDamage(float damage)
{
	_psDamage = damage;
}

void APhotonShot::SetSpeed(float speed)
{
	PhotonShotProjectile->Velocity = PhotonShotProjectile->Velocity * speed;
}

void APhotonShot::SetExplosionRange(float range)
{
	_psRange = range;
}

void APhotonShot::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TArray<AActor*> ignoreAI;
	ignoreAI.Add(bigowner);
	ignoreAI.Add(this);
	auto PhotonExplosion = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		PS_PhotonShotExplosion,
		GetActorLocation(),
		GetActorRotation(), true);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_PhotonHit, Hit.Location, 1.0f, 1.0f, 0.0f, nullptr);
	PhotonExplosion->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));

	if (Hit.GetActor())
	{
		ABarrier* Barrier = Cast<ABarrier>(Hit.GetActor());
		if (!Barrier)
		{
			UGameplayStatics::ApplyRadialDamage(GetWorld(), _psDamage, Hit.Location, _psRange, UDamageType::StaticClass(), ignoreAI, owner, owner->GetController(), false, ECollisionChannel::ECC_Pawn);
			Destroy();
		}
		else Destroy();
	}
}

