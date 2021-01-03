#include "HACKED_CRUSHER.h"
#include "CRUSHER_AnimInstance.h"
#include "InGame/Network/ReplicatableVar.h"
#include "InGame/Character/HACKED_CameraArmComponent.h"
#include "../../Stat/HpComponent.h"
#include "InGame/HACKEDTool.h"
#include "InGame/HACKEDInGameManager.h"
#include "Ingame/AI/HACKED_AI.h"
#include "Barrier.h"

using namespace AIAllStopTool;

AHACKED_CRUSHER::AHACKED_CRUSHER()
{
	PrimaryActorTick.bCanEverTick = true;

	Crusher_Stat = CreateDefaultSubobject<UCRUSHER_StatComponent>(TEXT("Crusher Stat Component"));
	rocketRushSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RocketRushSphere"));
	rocketRushSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	checkSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CHECK SPHERE"));
	checkSphere->InitSphereRadius(0.0f);
	jetPackEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("JET PACK EFFECT COMP"));
	jetPackEffectComp->SetupAttachment(GetMesh(), FName("JetPack"));
	primaryAttackEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PRIMARY EFFECT COMP"));
	rr_Foot_DirtTrailComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RR FOOT DIRT EFFECT COMP"));
	rr_Foot_DirtTrailComp->SetupAttachment(GetCapsuleComponent());
	rr_Foot_DirtTrailComp->SetRelativeLocation(FVector(0.0f, 0.0f, -95.0f));
	rr_Collision_EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RR COLLISION EFFECT COMP"));
	//ult_Buff_EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ULT BUFF EFFECT COMP"));
	//ult_Buff_EffectComp->SetupAttachment(GetCapsuleComponent());
	//ult_Buff_EffectComp->SetWorldScale3D(FVector(0.8f,0.8f,0.8f));
	//Skeletal Mesh 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SK_BODY(TEXT("SkeletalMesh'/Game/Resources/Character/Crusher/FBX/CRUSHER.CRUSHER'"));
	if (SK_BODY.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_BODY.Object);
		GetMesh()->AddLocalOffset(FVector(0.0f, 0.0f, -96.0f));
		GetMesh()->AddLocalRotation(FRotator(0.0, -90.0f, 0.0f));
		GetCameraBoom()->SetDesiredArmLength(400.0f);
	}
	//Animation 설정
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_BP(TEXT("AnimBlueprint'/Game/Resources/Character/Crusher/FBX/Animations/AnimBP_CRUSHER.AnimBP_CRUSHER_C'"));
	if (ANIM_BP.Succeeded()) { GetMesh()->SetAnimInstanceClass(ANIM_BP.Class); }
	//Widget 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> aimWG(TEXT("WidgetBlueprint'/Game/Blueprint/UI/InGame/Crusher/Aim_Crusher.Aim_Crusher_C'"));
	if (aimWG.Succeeded()) { WG_Aim_Class = aimWG.Class; }
	static ConstructorHelpers::FClassFinder<UUserWidget> rocketrushWG(TEXT("WidgetBlueprint'/Game/Blueprint/UI/InGame/Crusher/RocketRushChargingCRUSHER.RocketRushChargingCRUSHER_C'"));
	if (rocketrushWG.Succeeded()) { WG_RocketRush_Class = rocketrushWG.Class; }
	static ConstructorHelpers::FClassFinder<UUserWidget> informationWG(TEXT("WidgetBlueprint'/Game/Resources/UI/InGame/Crusher/CrusherInfoWG.CrusherInfoWG_C'"));
	if (informationWG.Succeeded()) { WG_Info_Class = informationWG.Class; }
	//Camera Shake 설정
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_PRIMARY(TEXT("/Game/Blueprint/CameraShake/Crusher/CS_00_PrimaryAttack.CS_00_PrimaryAttack_C"));
	if (CS_PRIMARY.Succeeded()) { CS_CrusherPA = CS_PRIMARY.Class; }
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_ROCKETRUSH(TEXT("/Game/Blueprint/CameraShake/Crusher/CS_01_RocketRush.CS_01_RocketRush_C"));
	if (CS_ROCKETRUSH.Succeeded()) { CS_RocketRush = CS_ROCKETRUSH.Class; }
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_GROUND_SMASH(TEXT("/Game/Blueprint/CameraShake/Crusher/CS_GroundSmash.CS_GroundSmash_C"));
	if (CS_GROUND_SMASH.Succeeded()) { CS_GroundSmash = CS_GROUND_SMASH.Class; }
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_CRUSHER_ULTIMATE_ATTACK(TEXT("/Game/Blueprint/CameraShake/Crusher/CS_UltimateAttack.CS_UltimateAttack_C"));
	if (CS_CRUSHER_ULTIMATE_ATTACK.Succeeded()) { CS_CrusherUltimateAttack = CS_CRUSHER_ULTIMATE_ATTACK.Class; }
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_ADRENAL_SURGE(TEXT("/Game/Blueprint/CameraShake/Crusher/CS_OnAdrenalSurge.CS_OnAdrenalSurge_C"));
	if (CS_ADRENAL_SURGE.Succeeded()) { CS_OnAdrenalSurge = CS_ADRENAL_SURGE.Class; }
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_VITALITYSHIELD(TEXT("/Game/Blueprint/CameraShake/Crusher/CS_05_VitalityShield.CS_05_VitalityShield_C"));
	if (CS_VITALITYSHIELD.Succeeded()) { CS_VitalityShield = CS_VITALITYSHIELD.Class; }
	//Effect 설정
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_JETPACK(TEXT("ParticleSystem'/Game/Resources/Character/Crusher/P_jetpack.P_jetpack'"));
	if (PS_JETPACK.Succeeded()) { jetPackEffect = PS_JETPACK.Object; jetPackEffectComp->SetTemplate(jetPackEffect); }
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_PRIMARY_HIT(TEXT("ParticleSystem'/Game/Resources/Character/Crusher/FX/Primary/P_Primary_CrusherHit.P_Primary_CrusherHit'"));
	if (PS_PRIMARY_HIT.Succeeded()) { primaryAttackEffect = PS_PRIMARY_HIT.Object; primaryAttackEffectComp->SetTemplate(primaryAttackEffect); }
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ROCKETRUSH_FOOT(TEXT("ParticleSystem'/Game/Resources/Character/Crusher/FX/M2/Crusher_M2Foot_DirtTrail.Crusher_M2Foot_DirtTrail'"));
	if (PS_ROCKETRUSH_FOOT.Succeeded()) { rr_Foot_DirtTrail = PS_ROCKETRUSH_FOOT.Object; rr_Foot_DirtTrailComp->SetTemplate(rr_Foot_DirtTrail); }
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ROCKETRUSH_COLLISION(TEXT("ParticleSystem'/Game/Resources/Character/Crusher/FX/M2/Crusher_M2_collision_Effect.Crusher_M2_collision_Effect'"));
	if (PS_ROCKETRUSH_COLLISION.Succeeded()) 
	{	
		rr_Collision_Effect = PS_ROCKETRUSH_COLLISION.Object; 
		rr_Collision_EffectComp->SetTemplate(rr_Collision_Effect); 
	}
	//static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ULTIMATE_BUFF(TEXT("ParticleSystem'/Game/Resources/Character/Crusher/P_Ultimate_Buff.P_Ultimate_Buff'"));
	//if (PS_ULTIMATE_BUFF.Succeeded()) { ult_Buff_Effect = PS_ULTIMATE_BUFF.Object; ult_Buff_EffectComp->SetTemplate(ult_Buff_Effect); }

	//Sound 설정
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_RR_DASH(TEXT("SoundWave'/Game/Sound/Crusher/RocketRush/Craher_Skill_RocketRush_Dash.Craher_Skill_RocketRush_Dash'"));
	if (SW_RR_DASH.Succeeded()) { rr_DashSound = SW_RR_DASH.Object; }
	static ConstructorHelpers::FObjectFinder<USoundWave> SW_RR_BOOSTER(TEXT("SoundWave'/Game/Sound/Crusher/RocketRush/Craher_Skill_RocketRush_booster.Craher_Skill_RocketRush_booster'"));
	if (SW_RR_BOOSTER.Succeeded()) { rr_BoosterSound = SW_RR_BOOSTER.Object; }


	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SA_ROCKETRUSH(TEXT("SoundAttenuation'/Game/Sound/Attenuations/CRUSHER/CRUSHER_ROCKETRUSH.CRUSHER_ROCKETRUSH'"));
	if (SA_ROCKETRUSH.Succeeded()) { rr_Attenuation = SA_ROCKETRUSH.Object; }

}

void AHACKED_CRUSHER::Possessed(AController* NewController)
{
	Super::Possessed(NewController);
	if (Cast<APlayerController>(NewController) && NetBaseCP->HasAuthority()) {
		//Aim 
		if (!WG_Aim) WG_Aim = CreateWidget<UUserWidget>(GetWorld(), WG_Aim_Class);
		if (!WG_Aim->IsInViewport()) WG_Aim->AddToViewport();
		if (!WG_RocketRush) WG_RocketRush = CreateWidget<UUserWidget>(GetWorld(), WG_RocketRush_Class);
	}
}

void AHACKED_CRUSHER::UnPossessed()
{
	Super::UnPossessed();
	if (WG_Aim && NetBaseCP->HasAuthority())
	{
		if (WG_Aim->IsInViewport()) WG_Aim->RemoveFromViewport();
		WG_Aim = nullptr;
	}
}

void AHACKED_CRUSHER::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void AHACKED_CRUSHER::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	animInstance = Cast<UCRUSHER_AnimInstance>(GetMesh()->GetAnimInstance());
	CHECK(animInstance);
	RocketRushEffectStop();
	JetPackEffectStop();
	UltimateBuffEffectStop();
	CameraBoom->SetDesiredSocketOffset(FVector(0.0f, 0.0f, 85.0f), 5.0f);
	InitSpCostStatByType(EHackedHeroType::CRUSHER); // 에스퍼 Sp 시스템을 적용합니다. 
	HpComponent->OnDie.AddUObject(this, &AHACKED_CRUSHER::RPCSelfHealing);
	rocketRushSphere->OnComponentBeginOverlap.AddDynamic(this, &AHACKED_CRUSHER::RocketRushOverlap);

	animInstance->PunishShieldDamageCheck.AddUObject(this, &AHACKED_CRUSHER::PunishShieldDamage);
	animInstance->PunishShieldEndCheck.AddUObject(this, &AHACKED_CRUSHER::PunishShieldEnd);
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AHACKED_CRUSHER::RocketRushHit);
	animInstance->AdrenalSurgeCheck.AddUObject(this, &AHACKED_CRUSHER::OnAdrenalSurge);
	animInstance->VitalityShieldCheck.AddUObject(this, &AHACKED_CRUSHER::VitalityShieldStart);
	animInstance->CrusherSelfHealingEnd.AddUObject(this, &AHACKED_CRUSHER::SelfHealingAnimEnd);
	animInstance->CrusherDashEnd.AddUObject(this, &AHACKED_CRUSHER::CrusherDashFinish);
	//animInstance->CrusherDieEnd.AddUObject(this, &AHACKED_CRUSHER::BothDieProcess);
}

void AHACKED_CRUSHER::BeginPlay()
{
	Super::BeginPlay();

	HpComponent->SetMaxHP(_crusherMaxHp);

	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, Attack);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, BarrierGenerator);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, PunishShield);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, AdrenalSurge);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, CrusherDash);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, CrusherDashFinish);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, VitalityShield);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, RocketRushStart);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, RocketRushStop);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, RocketRushLaunch);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, RPCSelfHealing);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, RPCBothDieProcess);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, CrusherIntroSkipStart);
	BindRPCFunction(NetBaseCP, AHACKED_CRUSHER, CrusherIntroSkipStop);
}

void AHACKED_CRUSHER::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetM2SkillRate();
	GetFirstSkillRate();
	GetSecondSkillRate();
	GetThirdSkillRate();
	UltimateGauageSystem();
	CrusherIntroSkip();

	/*LOG_SCREEN("bIsAdrenalSurge : %d", bIsAdrenalSurge);*/
	if (NetBaseCP->HasAuthority()) {
		RocketRushCharging();
		RocketRushing();
		if (bIsCrusherDash) { CrusherDashing(); }
	}
	if (animInstance) { bIsAttackState = animInstance->GetAttackState(); }

	bIsAttackState = animInstance->GetAttackState();

}

void AHACKED_CRUSHER::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Attack
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::Attack);
	// Barrier Generator
	PlayerInputComponent->BindAction(TEXT("Barrier Generator"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::BarrierGenerator);
	// Punish Shield
	PlayerInputComponent->BindAction(TEXT("PunishShield"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::PunishShield);
	// AdrenalSurge (Ultimate)
	PlayerInputComponent->BindAction(TEXT("AdrenalSurge"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::AdrenalSurge);
	// VitalityShield
	PlayerInputComponent->BindAction(TEXT("VitalityShield"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::VitalityShield);
	// Rocket Rush Start
	PlayerInputComponent->BindAction(TEXT("RocketRush"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::RocketRushStart);
	// Rocket Rush Launch
	PlayerInputComponent->BindAction(TEXT("RocketRush"), EInputEvent::IE_Released, this, &AHACKED_CRUSHER::RocketRushLaunch);
	// Dash
	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::CrusherDash);

	//SkipIntro
	PlayerInputComponent->BindAction(TEXT("SkipIntro"), EInputEvent::IE_Pressed, this, &AHACKED_CRUSHER::CrusherIntroSkipStart);

	PlayerInputComponent->BindAction(TEXT("SkipIntro"), EInputEvent::IE_Released, this, &AHACKED_CRUSHER::CrusherIntroSkipStop);


}

void AHACKED_CRUSHER::CrusherIntroSkipStart()
{
	RPC(NetBaseCP, AHACKED_CRUSHER, CrusherIntroSkipStart, ENetRPCType::MULTICAST, true);

	bCanSkipIntro = true;
}

void AHACKED_CRUSHER::CrusherIntroSkip()
{
	if (!bCanSkipIntro) return;
	
	if (currentIntroSkipTime <= maxtIntroSkipTime)
		currentIntroSkipTime += GetWorld()->GetDeltaSeconds();
	else if (currentIntroSkipTime > 3.0f)
	{
		currentIntroSkipTime = 3.0f;
		bCanSkipIntro = false;
	}
}

void AHACKED_CRUSHER::CrusherIntroSkipStop()
{
	RPC(NetBaseCP, AHACKED_CRUSHER, CrusherIntroSkipStop, ENetRPCType::MULTICAST, true);

	bCanSkipIntro = false;
}


float AHACKED_CRUSHER::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsAdrenalSurge)
	{
		DamageAmount = DamageAmount * 0.4f;
	}
	return AHACKEDCharacter::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AHACKED_CRUSHER::IsOnSelfHealing()
{
	return animInstance->AnimIsCrusherSelfHealing || animInstance->AnimIsCrusherSelfHealingFail;
}

void AHACKED_CRUSHER::DisableAimWidget()
{
	if (WG_Aim) {
		if (WG_Aim->IsInViewport()) {
			WG_Aim->RemoveFromParent();
		}
	}
}

float AHACKED_CRUSHER::GetUltimateRemainGuage()
{
	if (bIsAdrenalSurge) {
		return 1.0f - ((_adrenalTime - GetWorldTimerManager().GetTimerRemaining(CrusherUltimateHandle)) / _adrenalTime);
	}
	else {
		return 0.0f;
	}
}

void AHACKED_CRUSHER::RocketRushOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If actor already hitted, just ignore
	if (rr_Hitted.Contains(OtherActor))
		return;
	if (!bIsRocketRushLaunchingState)
		return;

	if (!HasAuthority()) {
		LOG_ERROR("SEX");
	}
	if (OtherActor->IsA(AHACKED_AI::StaticClass()))
	{
		AHACKED_AI* enemy = Cast<AHACKED_AI>(OtherActor);
		FVector enemyPos = enemy->GetActorLocation();
		FVector playerPos = GetActorLocation();
		FVector pushDir = enemyPos - playerPos;
		pushDir = pushDir.GetUnsafeNormal();
		RocketRushEffectHit(OtherActor->GetActorLocation());
		enemy->NetTransformCP->PlayDisableSyncTimer(0.5f);
		enemy->HACKEDLaunchCharacter(pushDir * rr_PushPower, 0.2f);
		animInstance->PlayAttackHitSound();
		if (HasAuthority()) {
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_RocketRush, 1.0f);
		}
		FPlayerDamageEvent playerDamageEvent;
		playerDamageEvent.bCanChargeUlt = true;
		playerDamageEvent.bCanHitAnimation = true;
		OtherActor->TakeDamage(rr_Damage, playerDamageEvent, GetController(), this);
		rr_Hitted.Add(enemy);
	}
}

//void AHACKED_CRUSHER::RocketRushHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	//If actor already hitted, just ignore
//	if (rr_Hitted.Contains(OtherActor))
//		return;
//	if (!bIsRocketRushLaunchingState)
//		return;
//
//	if (OtherActor->IsA(AHACKED_AI::StaticClass()))
//	{
//		AHACKED_AI* enemy = Cast<AHACKED_AI>(OtherActor);
//		FVector enemyPos = enemy->GetActorLocation();
//		FVector playerPos = GetActorLocation();
//		FVector pushDir = enemyPos - playerPos;
//		pushDir = pushDir.GetUnsafeNormal();
//		RocketRushEffectHit(Hit.ImpactPoint);
//		enemy->HACKEDLaunchCharacter(pushDir * rr_PushPower, 0.2f);
//		animInstance->PlayAttackHitSound();
//		if (HasAuthority()) {
//			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_RocketRush, 1.0f);
//		}
//		FPlayerDamageEvent playerDamageEvent;
//		playerDamageEvent.bCanChargeUlt = true;
//		playerDamageEvent.bCanHitAnimation = true;
//		OtherActor->TakeDamage(rr_Damage, playerDamageEvent, GetController(), this);
//		rr_Hitted.Add(enemy);
//	}
//}

void AHACKED_CRUSHER::RocketRushStart()
{
	if (NetBaseCP->HasAuthority())
	{
		if (!_bCanAttack || !bIsM2CoolDown || OnJump() ||
			bIsRocketRushLaunchingState || !bCanM2Skill)
			return;
		SkillCoolApplySystem(ESkillNumberType::M2Skill, false);
		RPC(NetBaseCP, AHACKED_CRUSHER, RocketRushStart, ENetRPCType::MULTICAST, true);
	}
	//If already in rushing state do not enter this function
	SetEnableInput(true, true, false);
	bIsRocketRushChargingState = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	if (WG_RocketRush && !WG_RocketRush->IsInViewport()) WG_RocketRush->AddToViewport();
}

void AHACKED_CRUSHER::RocketRushCharging()
{
	if (!bIsRocketRushChargingState)
		return;

	if (rr_Charging <= 2.0f)
		rr_Charging += GetWorld()->GetDeltaSeconds();
	else
	{
		rr_Charging = 2.0f;
		RocketRushLaunch();
	}
}

void AHACKED_CRUSHER::RocketRushLaunch()
{
	if (NetBaseCP->HasAuthority()) {
		//If already in rushing state do not enter this function
		if (bIsRocketRushLaunchingState)
			return;
		//If charging didn't reach the minimum value
		if (rr_Charging <= rr_MinCharging) {
			RocketRushStop();
			return;
		}

		RPC(NetBaseCP, AHACKED_CRUSHER, RocketRushLaunch, ENetRPCType::MULTICAST, true);
	}
	//Calculate rush distance
	if (rr_Charging >= 1.0f) { rr_RushDistance = rr_MaxRushDistance; }
	else { rr_RushDistance = rr_Charging * rr_MaxRushDistance; }
	_bCanTurn = false;
	_bCanMove = false;
	HLOG("distance : %d", rr_RushDistance);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	bIsRocketRushChargingState = false;
	bIsRocketRushLaunchingState = true;
	rr_StartLocation = GetActorLocation();
	HLOG("%s", *rr_StartLocation.ToString());
	rr_ForwardVector = GetActorForwardVector();
	HLOG("%s", *rr_ForwardVector.ToString());
	if (WG_RocketRush && WG_RocketRush->IsInViewport()) WG_RocketRush->RemoveFromViewport();
	RocketRushEffectStart();
	JetPackEffectStart();
	RocketRushSoundPlay();
}

void AHACKED_CRUSHER::RocketRushing()
{
	if (!bIsRocketRushLaunchingState)
		return;
	rr_checkWaitTime += GetWorld()->GetDeltaSeconds();
	if (rr_checkWaitTime >= rr_checkWaitMaxTime && !RocketRushingCheck()) {
		RocketRushStop();
		return;
	}
	GetCharacterMovement()->Velocity = rr_ForwardVector * rr_RushSpeed;
}

bool AHACKED_CRUSHER::RocketRushingCheck() const
{
	//If actor collide something and velocity isn't enough cancel rush
	if (GetCharacterMovement()->Velocity.Size() <= rr_MinVelocity)
		return false;
	//If actor rush enough distance cancel rush
	if (FVector::Dist(rr_StartLocation, GetActorLocation()) >= rr_RushDistance)
		return false;
	//If everything ok. keep doing it
	return true;
}

void AHACKED_CRUSHER::RocketRushStop()
{
	RPC(NetBaseCP, AHACKED_CRUSHER, RocketRushStop, ENetRPCType::MULTICAST, true);
	HLOG("ROCKET RUSH STOPPED!");
	bIsRocketRushLaunchingState = false;
	bIsRocketRushChargingState = false;
	if (!bIsSelfHealing)
	{
		SetEnableInput();
	}
	if (rr_Charging >= rr_MinCharging)
		GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	rr_RushDistance = 0.0f;
	rr_Charging = 0.0f;
	rr_checkWaitTime = 0.0f;
	rr_StartLocation = FVector::ZeroVector;
	rr_ForwardVector = FVector::ZeroVector;
	rr_Hitted.Empty();
	if (WG_RocketRush && WG_RocketRush->IsInViewport()) WG_RocketRush->RemoveFromViewport();
	RocketRushEffectStop();
	JetPackEffectStop();
}

void AHACKED_CRUSHER::RocketRushEffectHit(FVector location)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), rr_Collision_Effect, location, FRotator::ZeroRotator, true);
}

void AHACKED_CRUSHER::RocketRushEffectStart()
{
	rr_Foot_DirtTrailComp->Activate();
	rr_Foot_DirtTrailComp->ActivateSystem();
}

void AHACKED_CRUSHER::RocketRushEffectStop()
{
	rr_Foot_DirtTrailComp->DeactivateSystem();
	rr_Foot_DirtTrailComp->Deactivate();
}

void AHACKED_CRUSHER::RocketRushSoundPlay()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), rr_DashSound, GetActorLocation(), 1.f, 1.f, 0.f, rr_Attenuation);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), rr_BoosterSound, GetActorLocation(), 1.f, 1.f, 0.f, rr_Attenuation);

}

void AHACKED_CRUSHER::BarrierGenerator()
{
	if (NetBaseCP->HasAuthority())
	{
		if (!_bCanAttack || OnJump() || !bCanFirstSkill || 
			!bIsFirstCoolDown || animInstance->GetBarrierGeneratorActive())
			return;
		if (bIsAttackState) return;
		TArray<AActor*> outBarriers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABarrier::StaticClass(), outBarriers);
		if (outBarriers.Num() != 0) {
			return;
		}

		SkillCoolApplySystem(ESkillNumberType::FirstSkill, false);
		RPC(NetBaseCP, AHACKED_CRUSHER, BarrierGenerator, ENetRPCType::MULTICAST, true);
	}

	SetEnableInput(false,true,false);
	animInstance->SetBarrierGeneratorActive(true);
}

void AHACKED_CRUSHER::PunishShield()
{
	if (NetBaseCP->HasAuthority())
	{
		if (!_bCanAttack || OnJump() || !bCanSecondSkill ||
			!bIsSecondCoolDown)
			return;
		if (bIsAttackState) return;
		

		SkillCoolApplySystem(ESkillNumberType::SecondSkill, false);
		RPC(NetBaseCP, AHACKED_CRUSHER, PunishShield, ENetRPCType::MULTICAST, true);
	}
	SetEnableInput(false, true, false);
	PunishShieldCheckAI.Empty();
	animInstance->PlayPunishMontage();
	JetPackEffectStart();
	//animInstance->AnimIsPunishShield = true;

}

void AHACKED_CRUSHER::PunishShieldDamage()
{
	JetPackEffectStop();
	TArray<FHitResult> HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * _punishShieldRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(300.0f),
		Params);

	TArray<AActor*> hitActors;
	if (bResult)
	{
		for (auto Hit : HitResult)
		{
			if (Hit.GetActor() && !hitActors.Contains(Hit.GetActor()))
			{
				AHACKED_AI* hackedAI = Cast<AHACKED_AI>(Hit.GetActor());
				if (!hackedAI) continue;
				PunishShieldCheckAI.Add(hackedAI);
				FPlayerDamageEvent damageEvent;
				damageEvent.bCanChargeUlt = true;
				hackedAI->NetTransformCP->PlayDisableSyncTimer(0.5f);
				hackedAI->TakeDamage(_punishShieldDamage, damageEvent, GetController(), this);
				FVector pushBack = (hackedAI->GetActorLocation() - GetActorLocation());
				pushBack.Normalize();
				hackedAI->LaunchCharacter(pushBack * 750.0f, false, false);
			}
		}
	}
	animInstance->PlayAttackHitSound();
	PunishShieldStun();
}

void AHACKED_CRUSHER::PunishShieldStun()
{
	if (NetBaseCP->HasAuthority() == false) {
		return;
	}
	for (int i = 0; i < PunishShieldCheckAI.Num(); ++i)
	{
		AHACKED_AI* stunAI = PunishShieldCheckAI[i];
		if (PunishShieldCheckAI[i]->bCanRigidity) {
			PunishShieldCheckAI[i]->RPCRigidityAI();
		}

	}
}

void AHACKED_CRUSHER::PunishShieldEnd()
{
	if (!bIsSelfHealing) {
		SetEnableInput(true, true, true);
	}
	animInstance->AnimIsPunishShield = false;
}

void AHACKED_CRUSHER::PunishDamageChange(float changeValue)
{
	_punishShieldDamage += changeValue;
}

void AHACKED_CRUSHER::AdrenalSurge()
{
	LOG_SCREEN("bCanUltimate : %d", bCanUltimate);
	if (NetBaseCP->HasAuthority()) {
		if (!bCanUltimate || !bUltimateDisable) return;
		if (bIsAttackState) return;
		RPC(NetBaseCP, AHACKED_CRUSHER, AdrenalSurge, ENetRPCType::MULTICAST, true);
	}

	UltimateGaugeReset();
	SetEnableInput(false, false, false);
	animInstance->AnimIsAdrenalSurge = true;
	if (bIsNowOnCooperationTuto) {
		LOG(Warning, "Crusher Tuto Ultimate Access");
		bCooperationUltimateCheck = true;
	}

	// UI에서 바로 적용이 되도록 버튼을 누르자마자 켭니다.
	bIsAdrenalSurge = true;
	FollowCamera->PostProcessSettings.SceneFringeIntensity = 2.0f;
}

void AHACKED_CRUSHER::OnAdrenalSurge()
{
	float playerDistance;
	playerDistance = FVector::Dist(GetActorLocation(), HACKEDInGameManager->GetEsper()->GetActorLocation());
	LOG_SCREEN("Distance Between Player : %f", playerDistance);
	if (playerDistance > linkedSkillRange)
	{
		bIsAdrenalSurge = true;
		AdrenalSurgeSpawnEmitter(true);
	}

	else if (playerDistance < linkedSkillRange)
	{
		bIsAdrenalSurge = true;
		HACKEDInGameManager->GetEsper()->bIsAdrenalSurge = true;
		AdrenalSurgeSpawnEmitter(true);
		HACKEDInGameManager->GetEsper()->AdrenalSurgeSpawnEmitter(false);
	}

	SetEnableInput();
	LOG_SCREEN("Crusher Ultimate On");
	GetWorldTimerManager().SetTimer(CrusherUltimateHandle, this, &AHACKED_CRUSHER::AdrenalSurgeEnd, _adrenalTime, false);
}

void AHACKED_CRUSHER::AdrenalSurgeEnd()
{
	bIsAdrenalSurge = false;
	HACKEDInGameManager->GetEsper()->bIsAdrenalSurge = false;
	AdrenalSurgeEndEmitter();
	HACKEDInGameManager->GetEsper()->AdrenalSurgeEndEmitter();
	LOG_SCREEN("Crusher Ultimate Off");
	FollowCamera->PostProcessSettings.SceneFringeIntensity = 0.0f;
	//UltimateBuffEffectStop();
}

void AHACKED_CRUSHER::SpawnUltimateDustEffectComp(FVector location)
{
	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ult_Dust_Effect, location, FRotator::ZeroRotator, true);
}

void AHACKED_CRUSHER::UltimateBuffEffectStart()
{
	//ult_Buff_EffectComp->Activate();
	//ult_Buff_EffectComp->ActivateSystem();
}

void AHACKED_CRUSHER::UltimateBuffEffectStop()
{
	//ult_Buff_EffectComp->DeactivateSystem();
	//ult_Buff_EffectComp->Deactivate();
}

void AHACKED_CRUSHER::VitalityShield()
{
	// 마스터 클라이언트 공통으로 계산해야하므로 위로 뺌.
	esperShieldAmount = ((HACKEDInGameManager->GetEsper()->GetHPComponent()->GetShieldAmount()) * 2) / 3;
	crusherShieldAmount = ((HpComponent->GetShieldAmount()) * 2) / 3;

	if (NetBaseCP->HasAuthority()) {
		float playerDistance;
		playerDistance = FVector::Dist(GetActorLocation(), HACKEDInGameManager->GetEsper()->GetActorLocation());
		if (playerDistance > linkedSkillRange)
		{
			ShowVitalityErrorWidget(true);
			return;
		}
		if (esperShieldAmount <= 0 && crusherShieldAmount <= 0)
		{
			ShowVitalityErrorWidget(false);
			return;
		}
		LOG_SCREEN("Crusher Shield Amount : %f", crusherShieldAmount);
		LOG_SCREEN("Esper Shield Amount : %f", esperShieldAmount);

		if (bIsVitalityShield || !bIsThirdCoolDown || bIsSelfHealing || !_bCanAttack || !bCanThirdSkill)
			return;

		RPC(NetBaseCP, AHACKED_CRUSHER, VitalityShield, ENetRPCType::MULTICAST, true);
		SkillCoolApplySystem(ESkillNumberType::ThirdSkill, false);
	}

	bIsVitalityShield = true;
	animInstance->AnimIsVitalityShield = true;
	if (bIsNowOnCooperationTuto)
	{
		tutoVitalityUsedCheck = true;
	}
}

void AHACKED_CRUSHER::VitalityShieldStart()
{
	if (crusherShieldAmount > 0)
		VitalityShieldSpawnEmitter();

	if (esperShieldAmount > 0)
		HACKEDInGameManager->GetEsper()->VitalityShieldSpawnEmitter();

	_vitalityTime = 3.0f;
	GetWorldTimerManager().SetTimer(VitalityShieldHandle, this, &AHACKED_CRUSHER::VitalityHealing, 1.0f, true);

}

void AHACKED_CRUSHER::VitalityHealing()
{
	_vitalityTime -= 1.0f;

	//if (crusherShieldAmount > 0)
	//	VitalityShieldSpawnEmitter();

	//if (esperShieldAmount > 0)
	//	HACKEDInGameManager->GetEsper()->VitalityShieldSpawnEmitter();

	HpComponent->SetHP(HpComponent->GetHP() + crusherShieldAmount);
	HACKEDInGameManager->GetEsper()->GetHPComponent()->SetHP(HACKEDInGameManager->GetEsper()->GetHPComponent()->GetHP() + esperShieldAmount);

	if (_vitalityTime <= 0)
	{
		//VitalityShieldEndEmitter();
		//HACKEDInGameManager->GetEsper()->VitalityShieldEndEmitter();
		VitalityShieldEnd();
		GetWorldTimerManager().ClearTimer(VitalityShieldHandle);
	}
}

void AHACKED_CRUSHER::VitalityShieldEnd()
{
	bIsVitalityShield = false;
}

void AHACKED_CRUSHER::CrusherDash()
{
	if (NetBaseCP->HasAuthority())
	{
		if (!bIsDashCoolDown || bIsSelfHealing || !bCanDashSkill) return;
		SkillCoolApplySystem(ESkillNumberType::DashSkill, false);
		RPC(NetBaseCP, AHACKED_CRUSHER, CrusherDash, ENetRPCType::MULTICAST, true);
	}

	bIsCrusherDash = true;
	animInstance->AnimIsCrusherDash = true;
	DashStartPos = GetActorLocation();
	if (!bOnMove) DashDirection = GetActorForwardVector();
	else DashDirection = (GetForwardSpeedRate() * GetActorForwardVector() +
		GetRightSpeedRate() * GetActorRightVector()).GetUnsafeNormal();
	_crusherDashPower = 2000.0f;

	if (bIsNowOnTutorial) {
		DashCount++;
	}
	JetPackEffectStart();
}

void AHACKED_CRUSHER::CrusherDashing()
{
	if (FVector::Dist(DashStartPos, GetActorLocation()) >= (_crusherDashDistance * 0.1f))
	{
		if (GetVelocity().Size() <= (_crusherDashPower * 0.5f))
			CrusherDashFinish();
		GetCharacterMovement()->StopMovementImmediately();
	}

	if (FVector::Dist(DashStartPos, GetActorLocation()) >= _crusherDashDistance)
	{
		CrusherDashFinish();
		GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		GetCharacterMovement()->Velocity = (DashDirection * _crusherDashPower);
	}
}

void AHACKED_CRUSHER::CrusherDashFinish()
{
	RPC(NetBaseCP, AHACKED_CRUSHER, CrusherDashFinish, ENetRPCType::MULTICAST, true);

	bIsCrusherDash = false;
	JetPackEffectStop();
}

void AHACKED_CRUSHER::RPCSelfHealing()
{
	RPC(NetBaseCP, AHACKED_CRUSHER, RPCSelfHealing, ENetRPCType::MULTICAST, true);
	bIsSelfHealing = true;

	// 자가 치유 PP를 설정한다.
	FollowCamera->PostProcessSettings.VignetteIntensity = 1.0;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	if (HACKEDInGameManager->GetEsper()->IsOnSelfHealing())
	{
		LOG_SCREEN("Both Die State On");
		SetEnableInput(false, false, false);
		RPCBothDieProcess();
		HACKEDInGameManager->GetEsper()->RPCBothDieProcess();
		BackToLobby();
	}
	else
	{
		animInstance->AnimIsCrusherSelfHealing = true;
		SetEnableInput(false, true, false);

		GetWorldTimerManager().SetTimer(SelfHealingTimer, this, &AHACKED_CRUSHER::SelfHealingEnd, selfHealingTime, false);
	}


}

void AHACKED_CRUSHER::SelfHealingEnd()
{
	// 둘다 죽었을 경우에는 무시한다.
	if (animInstance->AnimIsCrusherSelfHealingFail) return;

	animInstance->AnimIsCrusherSelfHealing = false;
}

void AHACKED_CRUSHER::SelfHealingAnimEnd()
{
	bIsSelfHealing = false;
	SetEnableInput();

	// 자가 치유 PP를 초기화 한다.
	HpComponent->SetMaxHP(_crusherMaxHp);
	FollowCamera->PostProcessSettings.VignetteIntensity = 0.0;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void AHACKED_CRUSHER::RPCBothDieProcess()
{
	RPC(NetBaseCP, AHACKED_CRUSHER, RPCBothDieProcess, ENetRPCType::MULTICAST, true);
	animInstance->AnimIsCrusherSelfHealingFail = true;
	animInstance->AnimIsCrusherDieProcess = true;
	DisableAimWidget();
}

int AHACKED_CRUSHER::GetSkillRate(int skill)
{
	switch (skill) {
	case 0: // 우클릭 스킬
		return GetM2SkillRate();
	case 1: // 숫자 1번 스킬
		return GetFirstSkillRate();
	case 2: // 숫자 2번 스킬
		return GetSecondSkillRate();
	case 3: // 숫자 3번 스킬
		return GetThirdSkillRate();
	case 4: // 대쉬 스킬
		return GetDashSkillRate();
	default:
		return 0;
	}
}

void AHACKED_CRUSHER::InitCrusherStat()
{
	crusherStat = Crusher_Stat->GetCrusherStat();
}

float AHACKED_CRUSHER::GetMaxHp()
{
	return crusherStat.maxHp;
}

void AHACKED_CRUSHER::JetPackEffectStart()
{
	jetPackEffectComp->Activate();
	jetPackEffectComp->ActivateSystem();
}

void AHACKED_CRUSHER::JetPackEffectStop()
{
	jetPackEffectComp->Deactivate();
	jetPackEffectComp->DeactivateSystem();
}

void AHACKED_CRUSHER::Attack()
{
	LOG(Warning, "bCanAttack : %d, bCanPrimary : %d", _bCanAttack, bCanPrimary);
	if (HasAuthority() && !_bCanAttack) return;
	RPC(NetBaseCP, AHACKED_CRUSHER, Attack, ENetRPCType::MULTICAST, true);

	if (bIsSelfHealing || !bCanPrimary)
		return;

	if (!animInstance->IsAttackMontagePlaying())
	{
		animInstance->PlayAttackMontage();
	}

	if (animInstance->CanDoNextAttack())
	{
		animInstance->DoNextAttack();
	}
}

void AHACKED_CRUSHER::SpawnPrimaryHitEffect(FVector location)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), primaryAttackEffect, location, FRotator::ZeroRotator, true);
}

void AHACKED_CRUSHER::RushDamageChange(float changeValue)
{
	rr_Damage = rr_Damage + changeValue;
}
