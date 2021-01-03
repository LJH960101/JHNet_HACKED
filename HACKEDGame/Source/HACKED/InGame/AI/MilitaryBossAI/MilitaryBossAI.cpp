// Fill out your copyright notice in the Description page of Project Settings.


#include "MilitaryBossAI.h"
#include "MachineGunBullet.h"
#include "HomingMissile.h"
#include "ElectroMagneticPulse.h"
#include "MilitaryBoss_AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "InGame/Stat/HpComponent.h"
#include "MB_AI_AnimInstance.h"
#include "Components/WidgetComponent.h"

// Sets default values
AMilitaryBossAI::AMilitaryBossAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(75.0f, 108.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MBAI_SkeletalMesh(TEXT("/Game/Resources/Enemy/AI/MilitaryBossAI/MilitaryBossAI_SK.MilitaryBossAI_SK"));
	if (MBAI_SkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MBAI_SkeletalMesh.Object);
	}

	BossSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("BOSS AUDIO COMP"));

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> MBAI_AnimationBP(TEXT("/Game/Resources/Enemy/AI/MilitaryBossAI/MilitaryBossAI_AnimationBP.MilitaryBossAI_AnimationBP_C"));
	if (MBAI_AnimationBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(MBAI_AnimationBP.Class);
	}

	static ConstructorHelpers::FClassFinder<AMachineGunBullet> BulletClass(TEXT("/Game/Blueprint/InGame/AI/BP_MachineGunBullet.BP_MachineGunBullet_C"));
	if (BulletClass.Succeeded())
	{
		BulletBP = BulletClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHomingMissile> HomingMissileClass(TEXT("/Game/Blueprint/InGame/AI/BP_HomingMissile.BP_HomingMissile_C"));
	if (HomingMissileClass.Succeeded())
	{
		HomingMissileBP = HomingMissileClass.Class;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> BULLETHIT(TEXT("/Game/Resources/Enemy/P_Gun_Hit_Muzzle_02.P_Gun_Hit_Muzzle_02"));
	PS_BulletHit = BULLETHIT.Object;

	PC_BulletMuzzleLeft = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet Muzzle Left"));
	PC_BulletMuzzleRight = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet Muzzle Right"));

	PC_PowerShield = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Immortal Power Shield"));
	PC_PowerShield->SetupAttachment(GetCapsuleComponent());

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_POWERSHIELD(TEXT("/Game/Resources/Enemy/Boss/SuperArmor/P_SuperArmor.P_SuperArmor"));
	if (PS_POWERSHIELD.Succeeded())
	{
		PS_PowerShield = PS_POWERSHIELD.Object;
		PC_PowerShield->SetTemplate(PS_PowerShield);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_BULLETMUZZLE(TEXT("/Game/Resources/Enemy/P_MuzzleFlash_loop.P_MuzzleFlash_loop"));
	if (PS_BULLETMUZZLE.Succeeded())
	{
		PS_BulletMuzzle = PS_BULLETMUZZLE.Object;
		PC_BulletMuzzleRight->SetTemplate(PS_BulletMuzzle);
		PC_BulletMuzzleLeft->SetTemplate(PS_BulletMuzzle);
		PC_BulletMuzzleRight->SetupAttachment(GetMesh(), FName("Right-Bullet-Hole"));
		PC_BulletMuzzleLeft->SetupAttachment(GetMesh(), FName("Left-Bullet-Hole"));
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_EXPLOSION(TEXT("/Game/HACKED_AI/Bomb_Mine/P_Explosion_V4.P_Explosion_V4"));
	if (PS_EXPLOSION.Succeeded())
	{
		PS_BossExplosion = PS_EXPLOSION.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave>SB_BOOM(TEXT("SoundWave'/Game/Sound/Object/ExplosiveObject/TNT_BOOM_01.TNT_BOOM_01'"));
	if (SB_BOOM.Succeeded())
	{
		BossExplosionSound = SB_BOOM.Object;
	}
	

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_HAMMERING(TEXT("/Game/Blueprint/CameraShake/Boss/CS_00_Hammering.CS_00_Hammering_C"));
	if (CS_HAMMERING.Succeeded())
	{
		CS_Hammering = CS_HAMMERING.Class;
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_SLAPRUSHING(TEXT("/Game/Blueprint/CameraShake/Boss/CS_01_SlapRushing.CS_01_SlapRushing_C"));
	if (CS_SLAPRUSHING.Succeeded())
	{
		CS_SlapRushing = CS_SLAPRUSHING.Class;
	}


	AIControllerClass = AMilitaryBoss_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


	//FOR BOSS
	HackedAIWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> hackedBossHpBar(TEXT("/Game/Blueprint/HACKED_AI_UI/HACKED_BOSS_UI.HACKED_BOSS_UI_C"));
	if (hackedBossHpBar.Succeeded())
	{
		HackedAIWidget->SetWidgetClass(hackedBossHpBar.Class);
		//HackedAIWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
		HackedAIWidget->SetHiddenInGame(true);
	}
}

// Called when the game starts or when spawned
void AMilitaryBossAI::BeginPlay()
{
	Super::BeginPlay();
	AIAllStopTool::AIDisable(this);
	bIsAIImmortal = true;
	PC_BulletMuzzleRight->Deactivate();
	PC_BulletMuzzleLeft->Deactivate();
	HpComponent->SetMaxHP(_maxHp);
	GetCharacterMovement()->MaxWalkSpeed = _moveSpeed;
	GetMesh()->SetVisibility(false); 

	_bAgroIsCrusher.Init(&NetBaseCP, FString("_bAgroIsCrusher"));
	_bAgroIsCrusher.SetReliable(true);
	
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, MagneticPulseOn);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, RPCHammerShot);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, MachineGun);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, HomingMissile);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, SlapRush);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, VortexBeam);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, SetImmortalMode);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, BossDieExplosion);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, RPCSetBossDifficulty);
	BindRPCFunction(NetBaseCP, AMilitaryBossAI, _SetChangeToNearTarget);
}

void AMilitaryBossAI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MB_AI_Anim = Cast<UMB_AI_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(nullptr != MB_AI_Anim);
	MB_AI_Anim->OnHammeringCheck.AddUObject(this, &AMilitaryBossAI::HammerShotDamaging);
	MB_AI_Anim->OnHammeringEnd.AddUObject(this, &AMilitaryBossAI::HammerShotEnd);
	MB_AI_Anim->OnMachineGunCheck.AddUObject(this, &AMilitaryBossAI::MachineGunShootingStart);
	MB_AI_Anim->OnMachineGunEnd.AddUObject(this, &AMilitaryBossAI::MachineGunEnd);
	MB_AI_Anim->OnMissileCheck.AddUObject(this, &AMilitaryBossAI::HomingMissileStart);
	MB_AI_Anim->OnMissileEnd.AddUObject(this, &AMilitaryBossAI::HomingMissileEnd);
	MB_AI_Anim->OnSlapRushCheck.AddUObject(this, &AMilitaryBossAI::SlapRushDamaging);
	MB_AI_Anim->OnSlapRushEnd.AddUObject(this, &AMilitaryBossAI::SlapRushFinish);
	MB_AI_Anim->OnMBAIRigidityEnd.AddUObject(this, &AMilitaryBossAI::RigidityEnd);
	MB_AI_Anim->OnMBAIBeHitEnd.AddUObject(this, &AMilitaryBossAI::OnEndHitAnimation);

	PC_PowerShield->Deactivate();
}

// Called every frame
void AMilitaryBossAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMilitaryBossAI::RPCSetBossDifficulty(int bossDifficulty)
{
	RPC(NetBaseCP, AMilitaryBossAI, RPCSetBossDifficulty, ENetRPCType::MULTICAST, true, bossDifficulty);

	bossDifficulty = FMath::Clamp(bossDifficulty, 1, 3);
	// 1~3 
	switch (bossDifficulty)
	{
	case 1:
		SetPatternFrequency(EBossDifficulty::EASY);
		break;
	case 2:
		SetPatternFrequency(EBossDifficulty::NORMAL);
		break;
	case 3:
		SetPatternFrequency(EBossDifficulty::HARD);
		break;
	default:
		break;
	}
}

void AMilitaryBossAI::SetPatternFrequency(const EBossDifficulty& bossDifficulty)
{
	switch (bossDifficulty)
	{
	case EBossDifficulty::EASY:
		_bulletFrequency = 0.025f;
		_hammerShotDamage = 150.0f;
		_bulletShotDamage = 2.0f;
		_missileExpDamage = 60.0f;
		patternDistance = 1000.0f;
		bIsEasyMode = true;
		bIsNormalMode = false;
		bIsHardMode = false;
		break;

	case EBossDifficulty::NORMAL:
		_bulletFrequency = 0.025f;
		_hammerShotDamage = 200.0f;
		_bulletShotDamage = 2.5f;
		_missileExpDamage = 80.0f;
		patternDistance = 800.0f;
		bIsEasyMode = false;
		bIsNormalMode = true;
		bIsHardMode = false;
		break;

	case EBossDifficulty::HARD:
		_bulletFrequency = 0.025f;
		_hammerShotDamage = 300.0f;
		_bulletShotDamage = 3.0f;
		_missileExpDamage = 100.0f;
		patternDistance = 600.0f;
		bIsEasyMode = false;
		bIsNormalMode = false;
		bIsHardMode = true;
		break;

	default:
		break;
	}
}

void AMilitaryBossAI::_SetChangeToNearTarget(bool isOn)
{
	RPC(NetBaseCP, AMilitaryBossAI, _SetChangeToNearTarget, ENetRPCType::MULTICAST, true, isOn);
	changeToNearTarget = isOn;
}

bool AMilitaryBossAI::ForcedNearestTarget(int percent)
{
	return (FMath::RandRange(1, 100 / percent) == 1 ? true : false);
}

void AMilitaryBossAI::RefreshAgroPlayer()
{
	if (EsperAgroGauge > CrusherAgroGauge)
	{
		if (HACKEDInGameManager->GetEsper()->bIsSelfHealing && bChangeTargetDelay) {
			bChangeTargetDelay = false;
			GetWorldTimerManager().SetTimer(ChangeToTargetHandle, this, &AMilitaryBossAI::DieAgroReset, 2.0f, false);
		}
		else if (bDieAgroReset) {
			EsperAgroGauge = 0.0f;
			if (!(HACKEDInGameManager->GetEsper()->bIsSelfHealing)) {
				bDieAgroReset = false;
				bChangeTargetDelay = true;
			}
			_bAgroIsCrusher = true;
			return;
		}

		_bAgroIsCrusher = false;
		return;
	}
	else  
	{
		if (HACKEDInGameManager->GetCrusher()->bIsSelfHealing && bChangeTargetDelay) {
			bChangeTargetDelay = false;
			GetWorldTimerManager().SetTimer(ChangeToTargetHandle, this, &AMilitaryBossAI::DieAgroReset, 2.0f, false);
		}
		else if (bDieAgroReset) {
			CrusherAgroGauge = 0.0f;
			if (!(HACKEDInGameManager->GetCrusher()->bIsSelfHealing)) {
				bDieAgroReset = false;
				bChangeTargetDelay = true;
			}
			_bAgroIsCrusher = false;
			return;

		}
		_bAgroIsCrusher = true;
		return;
	}	
}

AHACKEDCharacter* AMilitaryBossAI::GetAgroPlayer()
{
	// 마스터는 어그로를 한번 체크한다.
	if(NetBaseCP->HasAuthority()) RefreshAgroPlayer();

	if (*_bAgroIsCrusher)
		return HACKEDInGameManager->GetCrusher();
	else
		return HACKEDInGameManager->GetEsper();
}

void AMilitaryBossAI::DieAgroReset()
{
	bDieAgroReset = true;
}


void AMilitaryBossAI::HammerShot(UBehaviorTreeComponent* OwnerComp)
{
	GetCharacterMovement()->MaxWalkSpeed = _moveSpeed;
	savedCp = OwnerComp;
	RPCHammerShot();
}

void AMilitaryBossAI::RPCHammerShot()
{
	RPC(NetBaseCP, AMilitaryBossAI, RPCHammerShot, ENetRPCType::MULTICAST, true);
	if (IsDie()) return;
	RefreshAgroPlayer();
	bIsHammering = true;
	MB_AI_Anim->AnimIsHammering = true;
}

void AMilitaryBossAI::HammerShotDamaging()
{
	if (IsDie()) return;

	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * _hammerShotRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(400.0f),
		Params);


	TArray<AActor*> hitActors;
	for (auto Hit : HitResult)
	{
		if (Hit.GetActor() && !hitActors.Contains(Hit.GetActor()))
		{
			AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(Hit.GetActor());
			if (!hackedPC) continue;
			FDamageEvent DamageEvent;
			hackedPC->TakeDamage(_hammerShotDamage, DamageEvent, GetController(), this);
			hitActors.Add(Hit.GetActor());
		}
	}
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), CS_Hammering, GetActorLocation(), 2000.0f, 3000.0f, 1.0f, true);
}

void AMilitaryBossAI::HammerShotEnd()
{
	bIsHammering = false;
	MB_AI_Anim->AnimIsHammering = false;
	if (savedCp) OnHammerShotEnd.ExecuteIfBound(savedCp);
}

float AMilitaryBossAI::GetHammerShotRange()
{
	return _hammerShotRange;
}

void AMilitaryBossAI::MachineGun()
{
	if (IsDie()) return;
	GetCharacterMovement()->MaxWalkSpeed = _moveSpeed;
	RPC(NetBaseCP, AMilitaryBossAI, MachineGun, ENetRPCType::MULTICAST, true);
	RefreshAgroPlayer();
	bIsMachineGun = true;
	MB_AI_Anim->AnimIsMachineGun = true;
}

void AMilitaryBossAI::MachineGunShootingStart()
{
	PC_BulletMuzzleRight->Activate();
	PC_BulletMuzzleLeft->Activate();
	_machineGunTime = 3.0f;
	GetWorldTimerManager().SetTimer(MachineGunHandle, this, &AMilitaryBossAI::MachineGunDamaging, _bulletFrequency, true);
}

void AMilitaryBossAI::MachineGunDamaging()
{
	if (IsDie()) return;
	_machineGunTime = _machineGunTime - _bulletFrequency;

	if (_machineGunTime > 0.0f)
	{
		FVector BulletStartPos;

		if (BulletSpawnPosCheck) {
			BulletStartPos = GetMesh()->GetSocketLocation("Left-Bullet-Hole");
			BulletSpawnPosCheck = false;
		}
		else {
			BulletStartPos = GetMesh()->GetSocketLocation("Right-Bullet-Hole");
			BulletSpawnPosCheck = true;
		}
			
		FVector TargetPos = GetAgroPlayer()->GetActorLocation();
		FVector BulletTargetPos = TargetPos;
		TargetPos.X -= FMath::RandRange(-100.0f, 100.0f);
		TargetPos.Y -= FMath::RandRange(-100.0f, 100.0f);
		TargetPos.Z -= FMath::RandRange(-100.0f, 100.0f);
		FVector TargetingPos = TargetPos - BulletStartPos;
		TargetingPos.Normalize();
		FVector BulletTargetingPos = BulletTargetPos - BulletStartPos;
		BulletTargetingPos.Normalize();

		// 비쥬얼 용 
		FVector BulletEndPos = BulletStartPos + TargetingPos * 100000.0f;
		// 데미지 용
		FVector BulletTrailEndPos = BulletStartPos + BulletTargetingPos * 100000.0f;

		FHitResult Hit(ForceInit);

		FCollisionQueryParams BulletShotQueryParams(NAME_None, true, this);
		BulletShotQueryParams.AddIgnoredActor(this);

		bool bResult = GetWorld()->LineTraceSingleByChannel(Hit, BulletStartPos, BulletTrailEndPos, ECC_GameTraceChannel10, BulletShotQueryParams);

		AMachineGunBullet* BulletShotActor;

		if (Hit.GetActor())
		{
			AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(Hit.GetActor());

			if (hackedPC)
				UGameplayStatics::ApplyDamage(hackedPC, _bulletShotDamage, GetController(), this, UDamageType::StaticClass());

			FVector TargetLocation = BulletEndPos;
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
			BulletShotActor = GetWorld()->SpawnActor<AMachineGunBullet>(BulletBP, BulletStartPos, TargetRotation);
		}
		else
		{
			FVector TargetLocation = BulletEndPos;
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
			BulletShotActor = GetWorld()->SpawnActor<AMachineGunBullet>(BulletBP, BulletStartPos, TargetRotation);
		}

		if (!BulletShotActor) return;
		BulletShotActor->SetOwnerToMB(this);
		BulletShotActor->SetBulletSpeed(12000.0f);
		BulletShotActor->SetBulletShotDamage(_bulletShotDamage);
		GetMesh()->IgnoreActorWhenMoving(BulletShotActor, true);
		GetCapsuleComponent()->IgnoreActorWhenMoving(BulletShotActor, true);
	}
	else if (_machineGunTime <= 0.0f)
	{
		PC_BulletMuzzleRight->Deactivate();
		PC_BulletMuzzleLeft->Deactivate();
		GetWorldTimerManager().ClearTimer(MachineGunHandle);
	}


}

void AMilitaryBossAI::MachineGunEnd()
{
	bIsMachineGun = false;
	bOnEndMachineGun = true;
	MB_AI_Anim->AnimIsMachineGun = false;
}

void AMilitaryBossAI::HomingMissile()
{
	if (IsDie()) return;
	GetCharacterMovement()->MaxWalkSpeed = _moveSpeed;
	RPC(NetBaseCP, AMilitaryBossAI, HomingMissile, ENetRPCType::MULTICAST, true);
	LOG_SCREEN("HomingCheck");
	RefreshAgroPlayer();
	bIsHomingMissile = true;
	MB_AI_Anim->AnimIsHomingMissile = true;
}

void AMilitaryBossAI::HomingMissileStart()
{
	LOG_SCREEN("HomingLaunchStart");
	_missileLaunchTime = 3.0f;
	GetWorldTimerManager().SetTimer(HomingMissileHandle, this, &AMilitaryBossAI::HomingMissileLaunch, 0.3f, true);
}

void AMilitaryBossAI::HomingMissileLaunch()
{
	if (IsDie()) return;
	_missileLaunchTime = _missileLaunchTime - 0.3f;

	if (_missileLaunchTime > 0.0f)
	{
		LOG_SCREEN("HomingIsNowOnFiring");
		FVector LaunchStartPos;
		FRotator MissileRotation = FRotator(0.0f,0.0f,0.0f);

		if (MissileSpawnPosCheck) {
			LaunchStartPos = GetMesh()->GetSocketLocation("Left-Bullet-Hole");
			MissileSpawnPosCheck = false;
		}
		else {
			LaunchStartPos = GetMesh()->GetSocketLocation("Right-Bullet-Hole");
			MissileSpawnPosCheck = true;
		}
			

		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = Instigator;

		AHomingMissile* LaunchMissile = GetWorld()->SpawnActor<AHomingMissile>(HomingMissileBP, LaunchStartPos, MissileRotation, spawnParams);
		LaunchMissile->MilitaryBossAI = this;
		LaunchMissile->Target = GetAgroPlayer();
		LaunchMissile->SetMissileDamage(_missileExpDamage);

		if (LaunchMissile != nullptr)
		{
			LOG_SCREEN("Can't Launch Missile");
			FRotator missileRot = LaunchMissile->MissileMesh->GetComponentRotation();
			missileRot.Roll = 0.0f;
			missileRot.Pitch = -90.0f;
			missileRot.Yaw = 0.0f;
			LaunchMissile->MissileMesh->SetRelativeRotation(missileRot);
		}
	}
	else if (_missileLaunchTime <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(HomingMissileHandle);
	}

}

void AMilitaryBossAI::HomingMissileEnd()
{
	bIsHomingMissile = false;
	MB_AI_Anim->AnimIsHomingMissile = false;
	bOnEndHomingMissile = true;
}

void AMilitaryBossAI::SlapRush()
{
	RPC(NetBaseCP, AMilitaryBossAI, SlapRush, ENetRPCType::MULTICAST, true);

	if (IsDie()) return;
	RefreshAgroPlayer();
	bIsSlapRush = true;
	MB_AI_Anim->AnimIsSlapRush = true;


	//GetCharacterMovement()->MaxWalkSpeed = _moveSpeed * 2;
	SlapRushStartPos = GetActorLocation();
	SlapRushDirection = GetAgroPlayer()->GetActorLocation() - GetActorLocation();
	SlapRushDirection.Normalize();
}

void AMilitaryBossAI::SlapRushDamaging()
{
	if (IsDie()) return;
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * _hammerShotRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel10,
		FCollisionShape::MakeSphere(400.0f),
		Params);


	TArray<AActor*> hitActors;
	for (auto Hit : HitResult)
	{
		if (Hit.GetActor() && !hitActors.Contains(Hit.GetActor()))
		{
			AHACKEDCharacter* hackedPC = Cast<AHACKEDCharacter>(Hit.GetActor());
			if (!hackedPC) continue;
			FDamageEvent DamageEvent;
			hackedPC->TakeDamage(_hammerShotDamage, DamageEvent, GetController(), this);
			hitActors.Add(Hit.GetActor());
		}
	}
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), CS_SlapRushing, GetActorLocation(), 800.0f, 1200.0f, 1.0f, true);
}

void AMilitaryBossAI::SlapRushing()
{

}


void AMilitaryBossAI::SlapRushFinish()
{
	bIsSlapRush = false;
	MB_AI_Anim->AnimIsSlapRush = false;
	bOnEndSlapRush = true;
}


void AMilitaryBossAI::VortexBeam()
{
	RPC(NetBaseCP, AMilitaryBossAI, VortexBeam, ENetRPCType::MULTICAST, true);

	RefreshAgroPlayer();
	bIsVortexBeam = true;
	MB_AI_Anim->AnimIsVortexBeam = true;
}



void AMilitaryBossAI::OnEnableAI(bool isEnable)
{
	Super::OnEnableAI(isEnable);
	StateReset();
}

void AMilitaryBossAI::DieProcess()
{
	BossDieExplosion();
	StateReset();
	Super::DieProcess();
}

void AMilitaryBossAI::BeHitAnimation()
{
	if (!bCanBossBeHit) return;
	StateReset();
	Super::BeHitAnimation();
}

void AMilitaryBossAI::OnEndHitAnimation()
{
	bCanBossBeHit = false;
	Super::OnEndHitAnimation();
}

void AMilitaryBossAI::RigidityAI()
{
	if (!bCanBossRigidity) return;
	StateReset();
	Super::RigidityAI();
}

void AMilitaryBossAI::RigidityEnd()
{
	bCanBossRigidity = false;
	Super::RigidityEnd();
}

void AMilitaryBossAI::StateReset()
{

}

float AMilitaryBossAI::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (NetBaseCP->HasAuthority())
	{
		AHACKED_ESPER* Esper = Cast<AHACKED_ESPER>(DamageCauser);
		AHACKED_CRUSHER* Crusher = Cast<AHACKED_CRUSHER>(DamageCauser);

		if (Esper)
		{
		}
		else if (Crusher)
		{
			crusherAgroCount++;
			if (crusherAgroCount > 10) {
				_SetChangeToNearTarget(true);
				changeToNearTarget = true;
			}
			else if (crusherAgroCount > 15)
			{
				_SetChangeToNearTarget(false);
				crusherAgroCount = 0;
			}
		}

		if (HpComponent->GetHP() < HpComponent->GetMaxHp() * 0.7f && _bFirstImmortalCheck)
		{
			MagneticPulseOn();
			SetImmortalMode(true);
			_bFirstImmortalCheck = false;
		}
		else if (HpComponent->GetHP() < HpComponent->GetMaxHp() * 0.5f && _bSecondImmortalCheck)
		{
			MagneticPulseOn();
			SetImmortalMode(true);
			_bSecondImmortalCheck = false;
		}
		else if (HpComponent->GetHP() < HpComponent->GetMaxHp() * 0.3f && _bThirdImmortalCheck)
		{
			MagneticPulseOn();
			SetImmortalMode(true);
			_bThirdImmortalCheck = false;
		}
	}

	if (_bIsBossImmortal)
	{
		currentStack = 0.0f;
		DamageAmount = 0.0f;
	}


	return AHACKED_AI::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

}

float AMilitaryBossAI::GetBossHp()
{
	return _maxHp;
}

void AMilitaryBossAI::AddHPWidgetToViewport()
{
	HackedAIWidget->GetUserWidgetObject()->AddToViewport();
}

void AMilitaryBossAI::RemoveHPWidgetToViewport()
{
	HackedAIWidget->GetUserWidgetObject()->RemoveFromViewport();
}

void AMilitaryBossAI::PowerShieldDelay()
{
	GetWorldTimerManager().SetTimer(ShieldDamageHandle, this, &AMilitaryBossAI::PowerShieldDamage, _powerShieldDelay, false);
}

void AMilitaryBossAI::SetImmortalMode(bool isOn)
{
	RPC(NetBaseCP, AMilitaryBossAI, SetImmortalMode, ENetRPCType::MULTICAST, true, isOn);

	if (isOn) {
		_bIsBossImmortal = true;
		PC_PowerShield->Activate();
		BossSoundComp->SetSound(ImmortalCautionSound);
		BossSoundComp->Play();
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("ST10"));
	}
	else {
		_bIsBossImmortal = false;
		PC_PowerShield->Deactivate();
	}
}

void AMilitaryBossAI::PowerShieldDamage()
{
	FDamageEvent shieldDamageEvent;
	this->TakeDamage(1000.0f, shieldDamageEvent, GetController(), HACKEDInGameManager->GetEsper());
}

void AMilitaryBossAI::MagneticPulseOn()
{
	RPC(NetBaseCP, AMilitaryBossAI, MagneticPulseOn, ENetRPCType::MULTICAST, true);

	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AElectroMagneticPulse::StaticClass(), outActors);

	for (auto actor : outActors)
	{
		AElectroMagneticPulse* ElectroMagneticPulse = Cast<AElectroMagneticPulse>(actor);
		if (!ElectroMagneticPulse) continue;
		ElectroMagneticPulse->SetOutline(true);
	}
}

void AMilitaryBossAI::BossDieExplosion()
{
	RPC(NetBaseCP, AMilitaryBossAI, BossDieExplosion, ENetRPCType::MULTICAST, true);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_BossExplosion, GetActorLocation(), GetActorRotation(), FVector(4.0f,4.0f,4.0f), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BossExplosionSound, GetActorLocation(), 1.0f, 1.0f, 0.0f);
	SetActorLocation(TeleportLocation->GetActorLocation());
	GetMesh()->SetVisibility(false);
}


