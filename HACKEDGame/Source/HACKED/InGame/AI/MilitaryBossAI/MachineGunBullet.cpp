// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineGunBullet.h"
#include "InGame/Character/CRUSHER/Barrier.h"

// Sets default values
AMachineGunBullet::AMachineGunBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	onHit = false;

	PC_BulletParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletPC"));
	BulletSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BulletSphere"));
	BulletProjectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BulletProjectTile"));

	// °èÃþ ¼³Á¤
	/*/Game/Resources/Character/Esper/FX/Primary_Shockwave/P_Primary_Projectile.P_Primary_Projectile*/
	RootComponent = BulletSphere;
	PC_BulletParticle->SetupAttachment(BulletSphere);
	/*/Game/Resources/Enemy/P_Gun_Bullet.P_Gun_Bullet*/
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BULLETPROJECTILE(TEXT("/Game/Resources/Enemy/P_Gun_Bullet.P_Gun_Bullet"));
	if (BULLETPROJECTILE.Succeeded())
	{
		PC_BulletParticle->SetTemplate(BULLETPROJECTILE.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BULLETHIT(TEXT("/Game/Resources/Enemy/P_Gun_Hit_Muzzle_02.P_Gun_Hit_Muzzle_02"));
	PS_BulletHit = BULLETHIT.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_BULLETHITGROUND(TEXT("/Game/Sound/Boss_Machine_Gun_Attack/BulletHitGroundCue.BulletHitGroundCue"));
	if (SB_BULLETHITGROUND.Succeeded())
		SB_BulletHitGround = SB_BULLETHITGROUND.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_BULLETHITPC(TEXT("/Game/Sound/Boss_Machine_Gun_Attack/BulletHitPCCue.BulletHitPCCue"));
	if (SB_BULLETHITPC.Succeeded())
		SB_BulletHitPlayer = SB_BULLETHITPC.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_BULLETHITBARRIER(TEXT("/Game/Sound/Boss_Machine_Gun_Attack/BulletHitBarrierCue.BulletHitBarrierCue"));
	if (SB_BULLETHITBARRIER.Succeeded())
		SB_BulletHitBarrier = SB_BULLETHITBARRIER.Object;

	

	BulletProjectile->InitialSpeed = 1.0f;
	BulletProjectile->ProjectileGravityScale = 0.0f;

	BulletSphere->SetEnableGravity(false);
	BulletSphere->SetCollisionProfileName(FName("AIShooting"));
	BulletSphere->OnComponentHit.AddDynamic(this, &AMachineGunBullet::OnComponentHit);
}

void AMachineGunBullet::SetOwnerToMB(AMilitaryBossAI* OwningActor)
{
	owner = OwningActor;
}

void AMachineGunBullet::SetBulletShotDamage(float damage)
{
	_bulletDamage = damage;
}

void AMachineGunBullet::SetBulletSpeed(float speed)
{
	BulletProjectile->Velocity = BulletProjectile->Velocity * speed;
}

void AMachineGunBullet::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (onHit) return;

	class AHACKEDCharacter* HackedPC = Cast<AHACKEDCharacter>(OtherActor);
	class AStaticMeshActor* BackGroundObject = Cast<AStaticMeshActor>(OtherActor);
	class ABarrier* Barrier = Cast<ABarrier>(OtherActor);

	if (Hit.GetActor())
	{
		if (Barrier)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_BulletHitBarrier, Hit.Location, 1.0f, 1.0f, 0.0f, nullptr); 
		}
		else if (HackedPC)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_BulletHitPlayer, Hit.Location, 1.0f, 1.0f, 0.0f, nullptr);
		}
		else if (BackGroundObject)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_BulletHitGround, Hit.Location, 1.0f, 1.0f, 0.0f, nullptr);
		}
	}

	TArray<AActor*> ignoreAI;
	ignoreAI.Add(this);
	ignoreAI.Add(owner);

	auto BulletHit = UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		PS_BulletHit,
		Hit.Location,
		GetActorRotation(), true);

	BulletHit->SetWorldScale3D(FVector(0.2f, 0.2f, 0.2f));

	BulletSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletProjectile->Velocity = FVector::ZeroVector;
	SetLifeSpan(0.2f);
	onHit = true;
}

// Called when the game starts or when spawned
void AMachineGunBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMachineGunBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

