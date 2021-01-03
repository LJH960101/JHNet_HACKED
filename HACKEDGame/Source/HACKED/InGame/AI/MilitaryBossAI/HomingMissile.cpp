// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingMissile.h"
#include "Core/HACKEDGameInstance.h"
#include "InGame/Character/CRUSHER/Barrier.h"
#include "MilitaryBossAI.h"


AHomingMissile::AHomingMissile(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	MissileCollision = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("MissileCollision"));

	//MissileCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("MissileCollision"));
	// 계층 설정
	RootComponent = MissileCollision;

	MissileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MissileMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MissileObj(TEXT("/Game/Resources/Enemy/AI/MilitaryBossAI/MilitaryBoss_HomingMissile.MilitaryBoss_HomingMissile"));
	if (MissileObj.Succeeded())
	{
		MissileMesh->SetStaticMesh(MissileObj.Object);
		MissileMesh->SetupAttachment(RootComponent);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MISSILEEXPLOSION(TEXT("/Game/Resources/Enemy/Boss/P_Explode.P_Explode"));
	if(MISSILEEXPLOSION.Succeeded())
		PS_MissileExplosion = MISSILEEXPLOSION.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MISSILEBARRIEREXPLOSION(TEXT("/Game/Resources/Enemy/Boss/P_Explode_Air.P_Explode_Air"));
	if (MISSILEBARRIEREXPLOSION.Succeeded())
		PS_MissileBarrierExplosion = MISSILEBARRIEREXPLOSION.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MISSILEDECAL(TEXT("/Game/Resources/Decal/Ground_Decal/Decal_Ground_rock.Decal_Ground_rock"));
	if (MISSILEDECAL.Succeeded())
		DC_MissileDecal = MISSILEDECAL.Object;

	MissileProjectile = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("MissileProjectTile"));
	MissileProjectile->UpdatedComponent = MissileCollision;
	MissileProjectile->InitialSpeed = 1500.0f;
	MissileProjectile->MaxSpeed = 2500.0f;
	MissileProjectile->bRotationFollowsVelocity = true;
	MissileProjectile->bInitialVelocityInLocalSpace = true;
	MissileProjectile->bShouldBounce = false;
	MissileProjectile->bIsHomingProjectile = false;
	MissileProjectile->HomingAccelerationMagnitude = 0.0f;
	MissileProjectile->ProjectileGravityScale = 0.0f;
	MissileProjectile->Velocity = FVector(0.0f, 0.0f, 0.0f);

	MissileCollision->SetCollisionProfileName(FName("MB_Missile"));
	MissileCollision->OnComponentBeginOverlap.AddDynamic(this, &AHomingMissile::OnOverlapBegin);
	
	_hasTargetPosition = false;
	_hasNoTarget = false; 
	Target = nullptr;
	_delayTimer = 0.0f;
	_hasFinishedDelay = false;
	_lifeTimeCountTime = 15.0f;
	_bCanDestroy = false;
	_startMissileSpeed = 1000.0f;

	this->SetActorEnableCollision(false);

	PC_MissileTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Missile Trail"));
	PC_MissileTrail->SetupAttachment(RootComponent);

	PC_MissileBlast = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Missile Blast"));
	PC_MissileBlast->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_MISSILEBLAST(TEXT("ParticleSystem'/Game/Resources/Enemy/Boss/P_missile.P_missile'"));
	if (PS_MISSILEBLAST.Succeeded())
	{
		PS_MissileBlast = PS_MISSILEBLAST.Object;
		PC_MissileBlast->SetTemplate(PS_MissileBlast);
	}


	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_MISSILETRAIL(TEXT("/Game/Resources/Enemy/Boss/P_MissileTrails2.P_MissileTrails2"));
	if (PS_MISSILETRAIL.Succeeded())
	{
		PS_MissileTrail = PS_MISSILETRAIL.Object;
		PC_MissileTrail->SetTemplate(PS_MissileTrail);
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_HOMMINGMISSILE(TEXT("/Game/Blueprint/CameraShake/Boss/CS_02_HomingMissile.CS_02_HomingMissile_C"));
	if (CS_HOMMINGMISSILE.Succeeded())
	{
		CS_HomingMissile = CS_HOMMINGMISSILE.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_MISSILEEXLOSION1(TEXT("/Game/Sound/Boss_Missile_Attack/Creature_Boss_Missile_Boom_01.Creature_Boss_Missile_Boom_01"));
	if (SB_MISSILEEXLOSION1.Succeeded())
		SB_MissileExplosion1 = SB_MISSILEEXLOSION1.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_MISSILEEXLOSION2(TEXT("/Game/Sound/Boss_Missile_Attack/Creature_Boss_Missile_Boom_02.Creature_Boss_Missile_Boom_02"));
	if (SB_MISSILEEXLOSION2.Succeeded())
		SB_MissileExplosion2 = SB_MISSILEEXLOSION2.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_MISSILEEXLOSION3(TEXT("/Game/Sound/Boss_Missile_Attack/Creature_Boss_Missile_Boom_03.Creature_Boss_Missile_Boom_03"));
	if (SB_MISSILEEXLOSION3.Succeeded())
		SB_MissileExplosion3 = SB_MISSILEEXLOSION3.Object;

	static ConstructorHelpers::FObjectFinder<USoundAttenuation>SA_MISSILEEXLOSION(TEXT("/Game/Sound/Attenuations/AI/MilitarySoldierAI_Beam.MilitarySoldierAI_Beam"));
	if (SA_MISSILEEXLOSION.Succeeded())
		SA_MissileExplosionAttenuation = SA_MISSILEEXLOSION.Object;
	
}

// Called when the game starts or when spawned
void AHomingMissile::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(HACKEDInGameManager);

	PC_MissileTrail->Activate();

	if (!_hasTargetPosition)
	{	
		if (HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial)
		{
			MissileProjectile->Velocity = GetActorUpVector() * 500.0f;
		}
		else 
		{
			MissileProjectile->Velocity = GetActorUpVector() * 250.0f;
		}

		
		this->SetActorEnableCollision(false);
	}
}

// Called every frame
void AHomingMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_hasFinishedDelay)
	{
		DelayLogic(DeltaTime);
	}
	else
	{
		// IsValideLowLevel : 객체가 유효한지 검사 존재한다면 true
		// IsPendingKill : 객체가 파괴 되었다고 표시되면  true
		if (_hasTargetPosition)
		{
			if (Target != nullptr)
			{
				if (Target->IsValidLowLevel()) // 타겟이 유효한 오브젝트인지 체크 
				{
					if (!(HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial)) {
						Target = MilitaryBossAI->GetAgroPlayer();
					}
					else {
						Target = HACKEDInGameManager->GetCrusher();
					}
					
					FVector wantedDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
					wantedDir += Target->GetVelocity() * wantedDir.Size() / MissileProjectile->MaxSpeed;
					MissileProjectile->Velocity += wantedDir * MissileProjectile->MaxSpeed * DeltaTime;
				}
				else
				{
					if (!this->IsPendingKill())
						if (this->IsValidLowLevel())
							Destroy();
				}
			}
			else // Agro Detect 된 타겟이 아예 존재하지 않을경우 하늘로 떡상시킴 
			{
				if (_hasNoTarget)
				{
					MissileProjectile->Velocity = GetActorUpVector() * 1000.0f;
					_hasNoTarget = false;
				}
			}
		}

		_lifeTimeCountTime -= 1 * DeltaTime;

		if (_lifeTimeCountTime < 0.0f)
		{
			if (!_bCanDestroy)
			{
				_bCanDestroy = true;
				Target = nullptr;
				MissileExplosion();
			}
		}
	}
}

void AHomingMissile::UpdateTargetPlayer()
{
	if (!_hasTargetPosition)
	{
		if (Target->IsValidLowLevel())
		{
			LOG_SCREEN("Success Get Target");
			if (!(HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial)) {
				Target = MilitaryBossAI->GetAgroPlayer();
			}
			else {
				Target = HACKEDInGameManager->GetCrusher();
			}
			
			_hasTargetPosition = true;
			_hasNoTarget = false;

			FRotator rotVal = MissileMesh->GetComponentRotation();
			rotVal.Roll = 0.0f;
			rotVal.Pitch = -90.0f;
			rotVal.Yaw = 0.0f;
			MissileMesh->SetRelativeRotation(rotVal);
		}
		else
		{
			LOG_SCREEN("Fail To Get Target");
			Target = nullptr;
			_hasTargetPosition = true;
			_hasNoTarget = true;
		}
	}
}

void AHomingMissile::SetMissileDamage(float damage)
{
	_missileDamage = damage;
}

void AHomingMissile::DelayLogic(float time)
{
	if (!_hasFinishedDelay)
	{
		_delayTimer += 1 * time;

		if (_delayTimer > 0.4f)
		{
			if (_delayTimer > 0.45f)
			{
				UpdateTargetPlayer();
				this->SetActorEnableCollision(true);
				_hasFinishedDelay = true;
			}
		}
	}
}

void AHomingMissile::SetMissileSpeed(float speed)
{
	MissileProjectile->Velocity = MissileProjectile->Velocity * speed;
}

void AHomingMissile::MissileExplosion()
{
	switch (rand() % 2)
	{
	case 0:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_MissileExplosion1, GetActorLocation(), 1.0f, 1.0f, 0.0f, SA_MissileExplosionAttenuation);
		break;
	case 1:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_MissileExplosion2, GetActorLocation(), 1.0f, 1.0f, 0.0f, SA_MissileExplosionAttenuation);
		break;
	default:
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SB_MissileExplosion3, GetActorLocation(), 1.0f, 1.0f, 0.0f, SA_MissileExplosionAttenuation);
		break;
	}
	UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DC_MissileDecal, FVector(200.0f, 200.0f, 200.0f), GetActorLocation(), GetActorRotation(), 4.0f);
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), CS_HomingMissile, GetActorLocation(), 700.0f, 1000.0f, true);
	Destroy();
}

void AHomingMissile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& hitResult)
{
	class AHACKEDCharacter* HackedPC = Cast<AHACKEDCharacter>(OtherActor);
	class AStaticMeshActor* BackGroundObject = Cast<AStaticMeshActor>(OtherActor);
	class ABarrier* Barrier = Cast<ABarrier>(OtherActor);

	TArray<AActor*> ignoreAI;
	ignoreAI.Add(MilitaryBossAI);
	ignoreAI.Add(this);

	if (Barrier != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_MissileBarrierExplosion, GetActorLocation(), GetActorRotation(), true);

		if (this->IsValidLowLevel())
		{
			MissileExplosion();

			if (HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial) {
				HACKEDInGameManager->GetCrusher()->BarrierHitCount++;
				LOG(Warning, "Barrier Hit Count : %d", HACKEDInGameManager->GetCrusher()->BarrierHitCount);
			}

			if (!(HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial)) {
				UGameplayStatics::ApplyRadialDamage(GetWorld(), 0.0f, GetActorLocation(), 700.0f, UDamageType::StaticClass(), ignoreAI, MilitaryBossAI, MilitaryBossAI->GetController(), false, ECollisionChannel::ECC_Pawn);
			}
		}
	}

	if (BackGroundObject != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_MissileExplosion, GetActorLocation(), GetActorRotation(), true);

		LOG(Warning, "BackGround Hitted");
		if (this->IsValidLowLevel())
		{
			MissileExplosion();

			if (!(HACKEDInGameManager->GetCrusher()->bIsNowOnTutorial)) {
				UGameplayStatics::ApplyRadialDamage(GetWorld(), _missileDamage, GetActorLocation(), 700.0f, UDamageType::StaticClass(), ignoreAI, MilitaryBossAI, MilitaryBossAI->GetController(), false, ECollisionChannel::ECC_Pawn);
			}
			Destroy();
		}
	}
}