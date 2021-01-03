// Fill out your copyright notice in the Description page of Project Settings.

#include "HACKED_ESPER.h"
#include "InGame/AI/HACKED_AI.h"
#include "Kismet/KismetMathLibrary.h"
#include "Esper_PA_EnergyBall.h"
#include "Esper_PsychicDrop.h"
#include "../../Stat/HpComponent.h"
#include "Esper_AnimInstance.h"
#include "InGame/Character/HACKED_CameraArmComponent.h"
#include "AIController.h"
#include "../HACKED_PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/HACKEDTool.h"
#include "BrainComponent.h"
#include "Blueprint/UserWidget.h"
#include "ESPER_AnimInstance.h"
#include "AIController.h"

using namespace AIAllStopTool;

void AHACKED_ESPER::Possessed(AController* NewController)
{
	Super::Possessed(NewController);
	if (Cast<APlayerController>(NewController) && NetBaseCP->HasAuthority()) {
		if (!WG_Aim) WG_Aim = CreateWidget<UUserWidget>(GetWorld(), WG_Aim_Class);
		if (!WG_Aim->IsInViewport()) WG_Aim->AddToViewport();
	}
}

void AHACKED_ESPER::UnPossessed()
{
	Super::UnPossessed();

	if (WG_Aim && NetBaseCP->HasAuthority())
	{
		if(WG_Aim->IsInViewport()) WG_Aim->RemoveFromViewport();
		WG_Aim = nullptr;
	}
}

// Sets default values
AHACKED_ESPER::AHACKED_ESPER()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("ESPER"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -96.0f), FRotator(0.0f, -90.0f, 0.0f));

	Esper_Stat = CreateDefaultSubobject<UESPER_StatComponent>(TEXT("Esper Stat Component"));
	//PC_LinkBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Link Beam Particle Component"));


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/Resources/Character/Esper/FBX/NewEsper/ESPER_NEW.ESPER_NEW"));
	if (SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

    static ConstructorHelpers::FObjectFinder<UMaterial> PRIMARY_PARTS_MATERIAL(TEXT("/Game/Resources/Character/Esper/FBX/NewEsper/Material/M_Esper_Parts_002.M_Esper_Parts_002"));
	if (PRIMARY_PARTS_MATERIAL.Succeeded())
	{
		PrimaryPartsMaterial = PRIMARY_PARTS_MATERIAL.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> ULTIMATE_PARTS_MATERIAL(TEXT("/Game/Resources/Character/Esper/FBX/NewEsper/Material/M_Esper_Ultimate_Parts.M_Esper_Ultimate_Parts"));
	if (ULTIMATE_PARTS_MATERIAL.Succeeded())
	{
		UltimatePartsMaterial = ULTIMATE_PARTS_MATERIAL.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ULTIMATEDECAL(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Material_Inst_MI/Use_Esper_R/MM_Decal_Inst.MM_Decal_Inst"));
	if (ULTIMATEDECAL.Succeeded())
		DC_EsperUltimateDecal = ULTIMATEDECAL.Object;

	PrimaryMaterialInst = UMaterialInstanceDynamic::Create(UltimatePartsMaterial, GetMesh()->GetMaterial(1));
	UltimateMaterialInst = UMaterialInstanceDynamic::Create(UltimatePartsMaterial, GetMesh()->GetMaterial(1));

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	UClass* animClass;
	GetClassAsset(animClass, UAnimInstance, "/Game/Resources/Character/Esper/FBX/NewEsper/Esper_AnimationBP.Esper_AnimationBP_C");
	GetMesh()->SetAnimInstanceClass(animClass);

	/*GetObjectAsset(PA_ShockWave, UParticleSystem, "/Game/Resources/Character/Esper/FX/NEW_v2/2/P_Esper_02_Effect_Cast.P_Esper_02_Effect_Cast");*/
	GetObjectAsset(PA_ShockWave, UParticleSystem, "/Game/Resources/Character/FX_Ver_2/FX2_Particle/Esper_Beam/P_Esper_02_Effect_Beam.P_Esper_02_Effect_Beam");

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> PSYCHICSHOCKWAVEHIT(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/2/P_Esper_02_attackEffect.P_Esper_02_attackEffect"));
	//{
	//	PA_ShockWaveHit = PSYCHICSHOCKWAVEHIT.Object;
	//}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PSYCHICSHOCKWAVEHIT(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Esper_Beam/P_Esper_S02_Beam_End.P_Esper_S02_Beam_End"));
	{
		PA_ShockWaveHit = PSYCHICSHOCKWAVEHIT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_PSYCHICDROP(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/M2/Esper_M2_Effect_Cast.Esper_M2_Effect_Cast"));
	{
		PA_PsychicDrop = P_PSYCHICDROP.Object;
	}

	GetObjectAsset(PS_PsychicDropCharge, UParticleSystem, "/Game/Resources/Character/Esper/FX/NEW_v2/M2/Esper_01_Effect_Start.Esper_01_Effect_Start");

	GetObjectAsset(PS_EsperSelfHeal, UParticleSystem, "/Game/Resources/Character/Esper/FX/NEW_v2/Esper_Self-healing/P_Esper_SelfHealing.P_Esper_SelfHealing");

	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ESPERSELFHEAL(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/Esper_Self-healing/P_Esper_SelfHealing.P_Esper_SelfHealing"));
	{
		PS_EsperSelfHeal = PS_ESPERSELFHEAL.Object;
	}*/

	/*static ConstructorHelpers::FClassFinder<AEsper_PA_EnergyBall> _PsychicTrailClass(TEXT("/Game/Blueprint/InGame/Character/Esper/BP_PsychicDrop.BP_PsychicDrop_C"));
	if (_PsychicTrailClass.Succeeded()) {
		PsychicTrailClass = _PsychicTrailClass.Class;
	}*/

	GetClassAsset(EnergyBallClass, AEsper_PA_EnergyBall, "/Game/Blueprint/InGame/Character/Esper/BP_EnergyBall.BP_EnergyBall_C");

	GetClassAsset(PsychicTrailClass, AEsper_PsychicDrop, "/Game/Blueprint/InGame/Character/Esper/BP_PsychicDrop.BP_PsychicDrop_C");

	GetObjectAsset(PS_PsychicDash, UParticleSystem, "/Game/Resources/Character/Esper/FX/NEW_v2/Dash/P_Dash_trail.P_Dash_trail");

	static ConstructorHelpers::FClassFinder<AEsper_PA_EnergyBall> StaticEnergyBallClass(TEXT("/Game/Blueprint/InGame/Character/Esper/BP_EnergyBall.BP_EnergyBall_C"));
	if (StaticEnergyBallClass.Succeeded())
	{
		EnergyBallClass = StaticEnergyBallClass.Class;
	}
	///Game/Resources/Character/FX_Ver_2/FX2_Particle/Esper_Beam/P_Esper_02_Effect_Beam.P_Esper_02_Effect_Beam
	// Game / Resources / Character / FX_Ver_2 / FX2_Particle / Shield / P_BeamTest.P_BeamTest
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_LINKBEAM(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P_BeamTest.P_BeamTest"));
	if (PS_LINKBEAM.Succeeded())
	{
		PS_LinkBeam = PS_LINKBEAM.Object;
		//PC_LinkBeam->SetTemplate(PS_LinkBeam);
		//PC_LinkBeam->SetupAttachment(GetCapsuleComponent());
	}

	// ---------------------------------------------------------- Camera Shake -------------------------------------------------------- //
	static ConstructorHelpers::FClassFinder<UCameraShake> CS_ESPERPRIMARY(TEXT("/Game/Blueprint/CameraShake/Esper/CS_00_EsperPrimary.CS_00_EsperPrimary_C"));
	if (CS_ESPERPRIMARY.Succeeded())
	{
		CS_EsperPrimary = CS_ESPERPRIMARY.Class;
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_PSYCHICFORCE(TEXT("/Game/Blueprint/CameraShake/Esper/CS_01_PsychicForce.CS_01_PsychicForce_C"));
	if (CS_PSYCHICFORCE.Succeeded())
	{
		CS_PsychicForce = CS_PSYCHICFORCE.Class;
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_PSYCHICDROP(TEXT("/Game/Blueprint/CameraShake/Esper/CS_02_PsychicDrop.CS_02_PsychicDrop_C"));
	if (CS_PSYCHICFORCE.Succeeded())
	{
		CS_PsychicDrop = CS_PSYCHICDROP.Class;
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_PSYCHICSHOCKWAVE(TEXT("/Game/Blueprint/CameraShake/Esper/CS_03_PsychicShockWave.CS_03_PsychicShockWave_C"));
	if (CS_PSYCHICFORCE.Succeeded())
	{
		CS_PsychicShockWave = CS_PSYCHICSHOCKWAVE.Class;
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_PSYCHICOVERDRIVE(TEXT("/Game/Blueprint/CameraShake/Esper/CS_04_PsychicOverDrive.CS_04_PsychicOverDrive_C"));
	if (CS_PSYCHICOVERDRIVE.Succeeded())
	{
		CS_PsychicOverDrive = CS_PSYCHICOVERDRIVE.Class;
	}

	static ConstructorHelpers::FClassFinder<UCameraShake> CS_PSYCHICOVERDRIVEEND(TEXT("/Game/Blueprint/CameraShake/Esper/CS_04_PsychicOverDriveEnd.CS_04_PsychicOverDriveEnd_C"));
	if (CS_PSYCHICOVERDRIVEEND.Succeeded())
	{
		CS_PsychicOverDriveEnd = CS_PSYCHICOVERDRIVEEND.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> aimWG(TEXT("/Game/Blueprint/UI/InGame/Esper/Aim_Esper.Aim_Esper_C"));
	if (aimWG.Succeeded()) 
	{
		WG_Aim_Class = aimWG.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> informationWG(TEXT("/Game/Resources/UI/InGame/Esper/EsperInfo.EsperInfo_C"));
	if (informationWG.Succeeded()) {
		WG_Info_Class = informationWG.Class;
	}

	//-------- Esper Stat --------//
	_esperMaxHp = 700.0f;
	_esperMaxWalkSpeed = 600.0f;

	//-------- Esper PrimaryAttack --------//
	_primaryAttackDamage = 20.0f;
	_primaryAttackSPDamage = 10.0f;
	_primaryAttackSPRange = 400.0f;
	_primaryAttackRange = 10000.0f;
	_primaryAttackSpeed = 6500.0f;
	_primaryAttackLifeTime = 0.6f;
	bIsPrimaryAttacking = false;

	//-------- Skill 1 PsychicForce --------//
	_psychicForceDamage = 20.0f;
	_psychicForceRange = 500.0f;
	bIsPsychicForce = false;
	_stunDelayTime = 0.5f;

	//-------- Skill MR PsychicDrop --------//
	_psychicDropRange = 1800.0f;
	_psychicDropDamageRange = 750.0f;
	_psychicDropDamage = 200.0f;
	_psychicDropSpeed = 5000.0f;
	PsychicDropDamageCount = 0.8f;
	bActorHitCheck = false;

	//-------- Skill 2 PsychicShockWave --------//
	_shockWaveMaxTime = 3.0f;
	_shockWaveRadius = 50.0f;
	_shockWaveRange = 1500.0f;
	_shockWaveDamage = 15.0f;

	//--------- Skill 3 PsychicShield --------//

	bIsPsychicShield = false;
	_psychicShieldDistance = 4000.0f;
	_psychicShieldAmount = 200.0f;
	_psychicShieldTime = 5.0f;

	//-------- Ultimate PsychicOverDrive --------//
	_psychicOverDriveRange = 1500.0f;
	_psychicOverDriveDamage = 2000.0f;
	_ultimateDropDelay = 0.17f;

	// SpSystem
	CurrentSp = 0;


}

void AHACKED_ESPER::DisableAimWidget()
{
	if (WG_Aim) {
		if (WG_Aim->IsInViewport()) {
			WG_Aim->RemoveFromParent();
		}
	}
}


// Called when the game starts or when spawned
void AHACKED_ESPER::BeginPlay()
{
	Super::BeginPlay();
	HpComponent->SetMaxHP(_esperMaxHp);
	HpComponent->SetMaxShield(_psychicShieldAmount);
	HACKEDInGameManager->GetCrusher()->GetHPComponent()->SetMaxShield(_psychicShieldAmount);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, Esper_Attack);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, Esper_AttackEnd);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicForce);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicDrop);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicShockWave);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicOverDrive);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, RPCSelfHealing);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicShield);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicDash);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, RPCSpawnDropEmitter);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, RPCBothDieProcess);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, RPCDestroyShield);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, RPCCreatePsychicShield);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, PsychicDashFinish);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, EsperIntroSkipStart);
	BindRPCFunction(NetBaseCP, AHACKED_ESPER, EsperIntroSkipStop);
}

float AHACKED_ESPER::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsAdrenalSurge)
	{
		DamageAmount = DamageAmount * 0.4f;
	}

	if (bIsPsychicOverDrive) return HpComponent->GetHP();
	return AHACKEDCharacter::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// Called every frame
void AHACKED_ESPER::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//LOG_SCREEN("Crusher bCanUltimate : %d", bCanUltimate);
	//LOG_SCREEN("OverDrive bool check : %d", bIsPsychicOverDrive);
	//LinkBeamStart();
	UltimateGauageSystem();
	GetM2SkillRate();
	GetFirstSkillRate();
	GetSecondSkillRate();
	GetThirdSkillRate();
	EsperIntroSkip();

	if (bIsPsychicDash && HasAuthority())
	{
		PsychicDashing();
	}

	if (bIsPsychicShockWave) // 3번 스킬의 동작이 활성화되었는지? 
	{
		if (ShockWave) // 3번 스킬 이펙트가 Spawn 되었을경우 
		{
			ShockWaveSetTarget();
		}
	}


	if (bIsPsychicOverDrive && bOnPsychicOverDriveHovering) // 궁극기의 동작이 활성화 되었는지? 
	{
		TArray<FHitResult> DetectResult;
		FCollisionQueryParams Params(NAME_None, false, this);
		bool bResult = GetWorld()->SweepMultiByChannel(
			DetectResult,
			GetActorLocation(),
			OverDriveSavePos,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(_psychicOverDriveRange),
			Params);

		if (bResult)
		{
			for (auto Hit : DetectResult)
			{
				if (Hit.GetActor())
				{
					AHACKED_AI* hackedAI = Cast<AHACKED_AI>(Hit.GetActor());
					if (!hackedAI || UltimateCheckedAI.Contains(hackedAI)) continue;
					UltimateCheckedAI.Add(hackedAI);
				}
			}
		}

		for (int i = 0; i < UltimateCheckedAI.Num(); ++i) {
			AHACKED_AI* hackedAI = UltimateCheckedAI[i];

			if (hackedAI->IsDie()) {
				UltimateCheckedAI.Remove(hackedAI);
				--i;
				continue;
			}

			hackedAI->NetBaseCP->PlayStopSyncTime(1.0f);
			AIAllStopTool::AIDisable(hackedAI); // 궁극기 범위 내에 들어온 AI를 비활성화합니다. 
			// ----------궁극기 내의 적을 목표 높이까지 들어올립니다. ----------//
			FVector currentPos = hackedAI->GetActorLocation();
			FVector desiredZPos = currentPos;
			desiredZPos.Z = GetActorLocation().Z + 500.0f;

			hackedAI->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
			currentPos = FMath::VInterpTo(currentPos, desiredZPos, DeltaTime, 0.5f);
			hackedAI->SetActorLocation(currentPos);
		}
	}
}

void AHACKED_ESPER::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Esper_Anim = Cast<UEsper_AnimInstance>(GetMesh()->GetAnimInstance());
	InitEsperStat(); // 에스퍼 스텟을 적용합니다. 
	InitSpCostStatByType(EHackedHeroType::ESPER); // 에스퍼 Sp 시스템을 적용합니다. 

	CameraBackToOrigin();

	HpComponent->OnDie.AddUObject(this, &AHACKED_ESPER::RPCSelfHealing);

	// Esper Primary Attack Spawn 
	Esper_Anim->OnAttackCheck.AddUObject(this, &AHACKED_ESPER::OnAttackCheck);

	//------------ Skill(1). Psychic Force ------------//
	// 스킬 설명 : 에스퍼 주변의 적을 염동력으로 강하게 밀어내며, 약간의 피해를 줍니다. 
	Esper_Anim->OnPsychicForceCheck.AddUObject(this, &AHACKED_ESPER::PsychicForceDamaging); // HACKED_AI에 만든 HACKEDLaunchCharacter함수를 통해 적을 밀어내며 피해를 주는 함수. 
	Esper_Anim->OnPsychicForceEnd.AddUObject(this, &AHACKED_ESPER::OnPsychicForceEnd); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 수치 조정함수입니다. 

	//------------ Skill(M). Psychic Drop ------------//
	// 스킬 설명 : 에스퍼가 에이밍 상의 오브젝트에 에너지 구체를 붙인후 터뜨려 피해를 입히는 스킬입니다. (최대 사거리 15m) 
	Esper_Anim->OnPsychicDropCheck.AddUObject(this, &AHACKED_ESPER::PsychicDropSpawnEmitter); // 플레이어가 지정한 목표에 이펙트를 소환하기 위한 함수 (해당 함수 뒤엔 AfterDamage를 위해 PsychicDropDamaging이 호출됩니다.) 
	Esper_Anim->OnPsychicDropEnd.AddUObject(this, &AHACKED_ESPER::OnPsychicDropEnd); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 수치 조정함수입니다. 
	Esper_Anim->OnPsychicDropDamaging.AddUObject(this, &AHACKED_ESPER::OnPsychicDropDamaging); // 부착된 구체를 폭파시킵니다.
	
	//------------ Skill(2). Psychic ShockWave ------------//
	// 스킬 설명 : 에스퍼가 전방에 강력한 에너지빔을 발사합니다. 좌우 회전만으로 데미지를 입힐수 있습니다. 
	Esper_Anim->OnPsychicShockWave.AddUObject(this, &AHACKED_ESPER::ShockWaveSpawnEmitter); // 싸이킥 쇼크웨이브 이펙트를 소환합니다. 
	Esper_Anim->OnPsychicShockWave.AddUObject(this, &AHACKED_ESPER::ShockWaveDamagingStart); // 3초간 전방의 적에게 도트데미지를 입힙니다. 
	Esper_Anim->ShockWaveDamagingEnd.AddUObject(this, &AHACKED_ESPER::ShockWaveDamagingEnd); // 빔발사를 끝낸후 이펙트를 삭제합니다. 
	Esper_Anim->OnPsychicShockWaveAnimEnd.AddUObject(this, &AHACKED_ESPER::ShockWaveEnd); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 수치 조정함수입니다. 

	//------------ Skill(3). Psychic Shield ------------//
	Esper_Anim->OnPsychicShieldCheck.AddUObject(this, &AHACKED_ESPER::PsychicShieldOn);
	Esper_Anim->OnPsychicShieldAnimEnd.AddUObject(this, &AHACKED_ESPER::PsychicShieldAnimEnd);

	//------------ Ultimate(U). Psychic OverDrive ------------//
	// 스킬 설명 : 에스퍼 주변의 적을 제압해 들어올린후 강력한 염동력으로 적을 땅으로 꽂습니다. 
	Esper_Anim->OnPsychicOverDriveCamReturn.AddUObject(this, &AHACKED_ESPER::PsychicOverDriveCamReturn);
	Esper_Anim->OnPsychicOverDriveEndAction.AddUObject(this, &AHACKED_ESPER::PsychicOverDriveEndAction); // 바인드함수와 Tick에서 들어올린 적을 땅으로 꽂습니다. 
	Esper_Anim->OnPsychicOverDriveEnd.AddUObject(this, &AHACKED_ESPER::PsychicOverDriveEnd); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 수치 조정함수입니다. 

	// SelfHealing ------------------------------------
	Esper_Anim->OnEsperSelfHealingEnd.AddUObject(this, &AHACKED_ESPER::SelfHealingAnimEnd); // 자가치유에 성공해 기본 State로 돌아가기 위한 수치 조정함수입니다.

	// PsychicDash
	Esper_Anim->OnEsperDashEnd.AddUObject(this, &AHACKED_ESPER::PsychicDashFinish);
}

// Called to bind functionality to input
void AHACKED_ESPER::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::Esper_Attack);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &AHACKED_ESPER::Esper_AttackEnd);

	//------------ Skill(1). Psychic Force ------------//
	PlayerInputComponent->BindAction(TEXT("PsychicForce"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::PsychicForce);

	//------------ Skill(2). Psychic Drop ------------//
	PlayerInputComponent->BindAction(TEXT("PsychicDrop"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::PsychicDrop);

	//------------ Skill(3). Psychic ShockWave ------------//
	PlayerInputComponent->BindAction(TEXT("PsychicShockWave"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::PsychicShockWave);

	//------------ Skill(4). Psychic Shield ------------//
	PlayerInputComponent->BindAction(TEXT("PsychicShield"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::PsychicShield);

	//------------ Ultimate(U). Psychic OverDrive ------------//
	PlayerInputComponent->BindAction(TEXT("PsychicOverDrive"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::PsychicOverDrive);

	// Dash 
	PlayerInputComponent->BindAction(TEXT("Dash"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::PsychicDash);

	//SkipIntro
	PlayerInputComponent->BindAction(TEXT("SkipIntro"), EInputEvent::IE_Pressed, this, &AHACKED_ESPER::EsperIntroSkipStart);

	PlayerInputComponent->BindAction(TEXT("SkipIntro"), EInputEvent::IE_Released, this, &AHACKED_ESPER::EsperIntroSkipStop);

}

void AHACKED_ESPER::EsperIntroSkipStart()
{
	RPC(NetBaseCP, AHACKED_ESPER, EsperIntroSkipStart, ENetRPCType::MULTICAST, true);

	bCanSkipIntro = true;
}

void AHACKED_ESPER::EsperIntroSkip()
{
	if (!bCanSkipIntro) return;
	LOG(Warning, "%f", CurrentIntroTime());
	if (currentIntroSkipTime <= maxtIntroSkipTime)
		currentIntroSkipTime += GetWorld()->GetDeltaSeconds();
	else if (currentIntroSkipTime > 3.0f)
	{
		currentIntroSkipTime = 3.0f;
		bCanSkipIntro = false;
	}
}

void AHACKED_ESPER::EsperIntroSkipStop()
{
	RPC(NetBaseCP, AHACKED_ESPER, EsperIntroSkipStop, ENetRPCType::MULTICAST, true);

	bCanSkipIntro = false;
	//currentIntroSkipTime = 0.0f;
}

int AHACKED_ESPER::GetSkillRate(int skill)
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

void AHACKED_ESPER::InitEsperStat()
{
	esperStat = Esper_Stat->GetEsperStat();
}

float AHACKED_ESPER::GetMaxHp()
{
	return esperStat.maxHp;
}

float AHACKED_ESPER::GetprimaryAttackDamage()
{
	return esperStat.primaryAttackDamage;
}

float AHACKED_ESPER::GetPASPDamage()
{
	return esperStat.paspDamage;
}

float AHACKED_ESPER::GetPASPRange()
{
	return esperStat.paspRange;
}

float AHACKED_ESPER::GetPASpeed()
{
	return esperStat.paSpeed;
}

float AHACKED_ESPER::GetPsychicForceDamage()
{
	return esperStat.psychicForceDamage;
}

float AHACKED_ESPER::GetPsychicForceRange()
{
	return esperStat.psychicForceRange;
}

float AHACKED_ESPER::GetPsychicDropDamage()
{
	return esperStat.psychicDropDamage;
}

float AHACKED_ESPER::GetPsychicDropRange()
{
	return esperStat.psychicDropRange;
}

float AHACKED_ESPER::GetPsychicWaveDamage()
{
	return esperStat.psychicWaveDamage;
}

float AHACKED_ESPER::GetPsychicWaveRange()
{
	return esperStat.psychicWaveRange;
}
//-------------------------------------------------------------------------------------//


void AHACKED_ESPER::CameraBackToOrigin()
{
	CameraBoom->SetDesiredArmLength(225.0f, 5.0f);
	CameraBoom->SetDesiredSocketOffset(FVector(-75.0f, 65.0f, 60.0f), 5.0f);
}


void AHACKED_ESPER::Esper_Attack()
{
	RPC(NetBaseCP, AHACKED_ESPER, Esper_Attack, ENetRPCType::MULTICAST, true);
	if (bIsSelfHealing || !bCanPrimary) return;
	bOnPrimaryAttack = true;
}

void AHACKED_ESPER::Esper_AttackEnd()
{
	RPC(NetBaseCP, AHACKED_ESPER, Esper_AttackEnd, ENetRPCType::MULTICAST, true);
	bOnPrimaryAttack = false;
}

void AHACKED_ESPER::OnAttackCheck()
{
	FHitResult Hit(ForceInit);
	// 에임상의 적 혹은 오브젝트에 이펙트를 소환하기위한 Vector값을 구합니다. 
	FVector PrimaryStartLocation = FollowCamera->GetComponentLocation();
	FVector PrimaryEndLocation = PrimaryStartLocation + FollowCamera->GetForwardVector() * 10000.0f;
	FCollisionQueryParams SW_TraceParams(NAME_None, false, this);
	
	GetWorld()->LineTraceSingleByChannel(Hit, PrimaryStartLocation, PrimaryEndLocation, ECC_GameTraceChannel7, SW_TraceParams);

	FVector PrimarySpawnLocation = GetMesh()->GetSocketLocation(FName("Bip001-L-Hand"));

	if (NetBaseCP->HasAuthority()) GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_EsperPrimary, 1.0f);

	AEsper_PA_EnergyBall * EnergyBall;
	// 에임상에 있는 적에게 적중하기 위해 회전값을 변경합니다.
	if (Hit.GetActor()) {
		FVector TargetLocation = Hit.Location;
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PrimarySpawnLocation, TargetLocation);
		EnergyBall = GetWorld()->SpawnActor<AEsper_PA_EnergyBall>(EnergyBallClass,
				PrimarySpawnLocation,
				TargetRotation);
	}
	// 적중하지 않았다면 타겟 로케이션을 끝점으로 합니다.
	else
	{
		FVector TargetLocation = PrimaryEndLocation; 
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PrimarySpawnLocation, TargetLocation);
		EnergyBall = GetWorld()->SpawnActor<AEsper_PA_EnergyBall>(EnergyBallClass,
			PrimarySpawnLocation,
			TargetRotation);
	}

	EnergyBall->SetOwnerToEsper(this);
	EnergyBall->SetSpeed(_primaryAttackSpeed);
	EnergyBall->SetprimaryAttackDamage(_primaryAttackDamage);
	EnergyBall->SetSPDamage(_primaryAttackSPDamage);
	EnergyBall->SetSPRange(_primaryAttackSPRange);
	EnergyBall->SetLifeSpan(_primaryAttackLifeTime);
	GetMesh()->IgnoreActorWhenMoving(EnergyBall, true);
	GetCapsuleComponent()->IgnoreActorWhenMoving(EnergyBall, true);
}

void AHACKED_ESPER::PsychicDrop()
{
	// 나의 캐릭터라면 수행이 가능한지 먼저 조사합니다.
	if (NetBaseCP->HasAuthority()) {
		if (!_bCanAttack || OnJump()) return;
		if (!bIsM2CoolDown || !bCanM2Skill) return;
		SkillCoolApplySystem(ESkillNumberType::M2Skill, false);
	}

	RPC(NetBaseCP, AHACKED_ESPER, PsychicDrop, ENetRPCType::MULTICAST, true);
	bIsPsychicDrop = true;
	Esper_Anim->AnimIsPsychicDrop = true;
	SetEnableInput(false, true, false);
	//SetTurnRate(0.0f, 0.0f);
}

void AHACKED_ESPER::PsychicDropSpawnEmitter()
{
	if (!NetBaseCP->HasAuthority()) return;
	FHitResult Hit(ForceInit);
	PsychicDropStartVec = FollowCamera->GetComponentLocation();
	PsychicDropEndVec = PsychicDropStartVec + FollowCamera->GetForwardVector() * 10000.0f;

	FCollisionQueryParams PD_TraceParams(NAME_None, false, this);

	GetWorld()->LineTraceSingleByChannel(Hit, PsychicDropStartVec, PsychicDropEndVec, ECC_GameTraceChannel8, PD_TraceParams);

	//DrawDebugLine(GetWorld(), PsychicDropStartVec, PsychicDropEndVec, FColor::Green, false, 2.f, false, 4.f);

	FVector PsychicDropSpawnLocation = GetMesh()->GetSocketLocation("Bip001-R-Hand");


	if (Hit.GetActor())
	{
		FVector TargetLocation = Hit.Location;
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PsychicDropSpawnLocation, TargetLocation);
		RPCSpawnDropEmitter(TargetLocation, TargetRotation.Euler());

	}
	else
	{
		FVector TargetLocation = PsychicDropEndVec;
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PsychicDropSpawnLocation, TargetLocation);
		RPCSpawnDropEmitter(TargetLocation, TargetRotation.Euler());
	}
}

void AHACKED_ESPER::RPCSpawnDropEmitter(FVector targetPoint, FVector targetRotation)
{
	RPC(NetBaseCP, AHACKED_ESPER, RPCSpawnDropEmitter, ENetRPCType::MULTICAST, true, targetPoint, targetRotation);

	lastPhychicDrop = GetWorld()->SpawnActor<AEsper_PsychicDrop>(PsychicTrailClass,
		GetMesh()->GetSocketLocation("Bip001-R-Hand"),
		FRotator::MakeFromEuler(targetRotation));

	if (!lastPhychicDrop) return;
	lastPhychicDrop->SetOwnerToEsper(this);
	lastPhychicDrop->SetSpeed(_psychicDropSpeed);
	lastPhychicDrop->SetExplosionDamage(_psychicDropDamage);
	lastPhychicDrop->SetExplosionRange(_psychicDropDamageRange);
	GetMesh()->IgnoreActorWhenMoving(lastPhychicDrop, true);
	GetCapsuleComponent()->IgnoreActorWhenMoving(lastPhychicDrop, true);
}

void AHACKED_ESPER::OnPsychicDropDamaging()
{
	if (lastPhychicDrop) {
		lastPhychicDrop->PsychicDropExplosion();
		if (NetBaseCP->HasAuthority()) GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_PsychicDrop, 1.0f);
		lastPhychicDrop = nullptr;
	}
}

void AHACKED_ESPER::OnPsychicDropEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = _esperMaxWalkSpeed;
	bIsPsychicDrop = false;
	Esper_Anim->AnimIsPsychicDrop = false;
	if (!bIsSelfHealing)
	{
		SetEnableInput();
	}
	SetTurnRate();
}

void AHACKED_ESPER::PsychicDropDamageChange(float plusDamage)
{
	_psychicDropDamage += plusDamage;
}



void AHACKED_ESPER::PsychicForce()
{
	// 나의 캐릭터라면 수행이 가능한지 먼저 조사합니다.
	if (NetBaseCP->HasAuthority()) {
		if (!_bCanAttack || OnJump()) return;
		//if(!SpReduceApplyState(ESkillNumberType::M2Skill)) return;
		if (!bIsFirstCoolDown || !bCanFirstSkill) return;
		SkillCoolApplySystem(ESkillNumberType::FirstSkill, false);
	}

	RPC(NetBaseCP, AHACKED_ESPER, PsychicForce, ENetRPCType::MULTICAST, true);
	SetEnableInput(false, false, false);
	PsychicDropSavePos = GetActorLocation() + FVector(0.0f, 0.0f, 0.1f);
	CameraBoom->SetDesiredArmLength(800.0f, 10.0f);
	CameraBoom->SetDesiredSocketOffset(FVector(0.0f, 0.0f, 90.0f), 10.0f);
	PsychicForceCheckAI.Empty();
	bIsPsychicForce = true;
	if(NetBaseCP->HasAuthority()) GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_PsychicForce, 1.0f);
	Esper_Anim->AnimIsPsychicForce = true;
}

void AHACKED_ESPER::PsychicForceDamaging()
{
	TArray<FHitResult> DetectResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		DetectResult,
		GetActorLocation(),
		PsychicDropSavePos,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(_psychicForceRange),
		Params);

	if (bResult)
	{
		bool onSound = false;
		for (auto Hit : DetectResult)
		{
			if (Hit.GetActor())
			{
				LOG_SCREEN("Hit Result: %s", *(Hit.GetActor()->GetName()));
				AHACKED_AI* hackedAI = Cast<AHACKED_AI>(Hit.GetActor());
				if (!hackedAI) continue;
				PsychicForceCheckAI.Add(hackedAI);
				AIDisable(hackedAI);
				FVector pushBack = (hackedAI->GetActorLocation() - GetActorLocation());
				pushBack += FVector::UpVector * 0.1f;
				pushBack.Normalize();
				hackedAI->NetTransformCP->PlayDisableSyncTimer(0.5f);
				// BT의 MoveTo 상태인 적들은 언리얼 제공함수인 LaunchCharacter로 밀어지지 않아 HACKED_AI의 HACKEDLaunchCharacter에서 적들을 밀어줍니다. ;
				hackedAI->HACKEDLaunchCharacter(pushBack * 6000.0f, 0.2f, 0.1f); 
				FPlayerDamageEvent playerEvent;
				playerEvent.bCanChargeUlt = true;
				playerEvent.bCanExplosionEsperStack = true;
				hackedAI->TakeDamage(_psychicForceDamage, playerEvent, GetController(), this);
				if (!onSound) 
				{
					PlaySkill1Sound(hackedAI->GetActorLocation());
					onSound = true;
				}

			} 
		}
	}
	
	FollowCamera->PostProcessSettings.SceneFringeIntensity = 5.0f;
	GetWorldTimerManager().SetTimer(PsychicForceStunDelay, this, &AHACKED_ESPER::PsychicForceStun, _stunDelayTime * 0.5f, false);
}

void AHACKED_ESPER::PsychicForceStun()
{
	FollowCamera->PostProcessSettings.SceneFringeIntensity = 0.0f;
	for (int i = 0; i < PsychicForceCheckAI.Num(); ++i)
	{
		AHACKED_AI* Hit = PsychicForceCheckAI[i];
		
		if(!(Hit->IsDie()))
		PsychicForceCheckAI[i]->RPCBeHitAnimation();
	}
}

void AHACKED_ESPER::OnPsychicForceEnd()
{
	if (!bIsSelfHealing)
	{
		SetEnableInput();
	}
	bIsPsychicForce = false;
	CameraBackToOrigin();
}

void AHACKED_ESPER::PsychicForceDamageChange(float plusDamage)
{
	_psychicForceDamage += plusDamage;
}

void AHACKED_ESPER::PsychicShockWave()
{
	// 나의 캐릭터라면 수행이 가능한지 먼저 조사합니다.
	if (NetBaseCP->HasAuthority()) {
		if (!_bCanAttack || OnJump()) return;
		//	if(!SpReduceApplyState(ESkillNumberType::SecondSkill)) return;
		if (!bIsSecondCoolDown || !bCanSecondSkill) return;
		SkillCoolApplySystem(ESkillNumberType::SecondSkill, false);

		RPC(NetBaseCP, AHACKED_ESPER, PsychicShockWave, ENetRPCType::MULTICAST, true);
	}

	CameraBoom->SetDesiredArmLength(150.0f, 1.0f);
	CameraBoom->SetDesiredSocketOffset(FVector(-14.0f, 120.0f, 70.0f), 5.0f);
	//CameraBoom->SetDesiredSocketOffset(FVector(0.0f, 30.0f, 50.0f), 5.0f);

	SetTurnRate(7.5f, 0.0f);
	
	// 마우스의 상하 조작을 막습니다.
	if (GetController())
	{		
		FRotator rotation = GetController()->GetControlRotation();
		rotation.Pitch = 0.0f;
		GetController()->SetControlRotation(rotation);
	}
	bIsPsychicShockWave = true;
	Esper_Anim->AnimIsPsychicShockWave = true;
	SetEnableInput(true, true, false);
}

void AHACKED_ESPER::ShockWaveSpawnEmitter()
{
	if (ShockWave) {
		ShockWave->DestroyComponent();
		ShockWave = nullptr;
	}
	if (ShockWaveHit) {
		ShockWaveHit->DestroyComponent();
		ShockWaveHit = nullptr;
	}
	if (NetBaseCP->HasAuthority()) GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_PsychicShockWave, 1.0f);


	//ShockWave = UGameplayStatics::SpawnEmitterAttached(PA_ShockWave, GetMesh(), FName("None"), FVector(0.0f, 42.0f, 148.0f),  // 신채환 에스퍼 이펙트 Spawn 위치 
		//FRotator(0.0f, 95.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);

	//ShockWave->SetRelativeScale3D(FVector(1.75f, 1.75f, 1.75f));

	ShockWave = UGameplayStatics::SpawnEmitterAttached(PA_ShockWave, GetMesh(), FName("None"), FVector(0.0f, 42.0f, 138.0f),     // 보람 에스퍼 이펙트 Spawn 위치 
		FRotator(0.0f, 95.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);

	ShockWave->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	ShockWaveHit = UGameplayStatics::SpawnEmitterAttached(PA_ShockWaveHit, GetMesh(), FName("None"), FVector(0.0f, 30.0f, 110.0f),
		FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);
}
 
void AHACKED_ESPER::ShockWaveDamagingStart()
{
	_shockWaveTime = _shockWaveMaxTime;
	GetWorldTimerManager().SetTimer(ShockWaveTimerHandle, this, &AHACKED_ESPER::ShockWaveDamaging, 0.1f, true);
}

void AHACKED_ESPER::ShockWaveSetTarget()
{
	FHitResult Hit(ForceInit);
	FVector ShockWaveStartVec = FollowCamera->GetComponentLocation();
	FVector ShockWaveEndVec = ShockWaveStartVec + FollowCamera->GetForwardVector() * _shockWaveRange;
	FCollisionQueryParams SW_TraceParams(NAME_None, true, this);

	//DrawDebugLine(GetWorld(), ShockWaveStartVec, ShockWaveEndVec, FColor::Green, false, 2.f, false, 4.f);

	GetWorld()->LineTraceSingleByChannel(Hit, ShockWaveStartVec, ShockWaveEndVec, ECC_GameTraceChannel7, SW_TraceParams);

	FVector HitLocation;
	if (Hit.GetActor())
	{
		HitLocation = Hit.Location;
	}
	else HitLocation = ShockWaveEndVec;
	if (ShockWave) 
	{
		int32 NumberOfBeams = 32;
		for (int32 i = 0; i <= NumberOfBeams; i++)
		{
			ShockWave->SetBeamTargetPoint(i, HitLocation, 0);
		}

		ShockWave->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitLocation));
	}
	if (ShockWaveHit)
	{
		ShockWaveHit->SetWorldLocation(HitLocation);
	}
}

void AHACKED_ESPER::ShockWaveDamaging()
{
	_shockWaveTime -= 0.1f;

	if (_shockWaveTime >= 2.9f)
	{
		FollowCamera->PostProcessSettings.SceneFringeIntensity = 3.0f;
		FollowCamera->PostProcessSettings.BloomIntensity = 3.0f;
		FollowCamera->PostProcessSettings.BloomThreshold = -1.0f;
	}

	else if (_shockWaveTime > 0.0f)
	{
		TArray<FOverlapResult> HitResult;
		FCollisionQueryParams SW_HitParams(NAME_None, false, this);

		FVector capsuleLocation = GetActorLocation() + GetActorForwardVector() * _shockWaveRange;
		FVector lookAtFevtor = UKismetMathLibrary::FindLookAtRotation(capsuleLocation, GetActorLocation()).Euler();
		lookAtFevtor.Y = lookAtFevtor.Y + 90.0f; 
		FQuat capsuleRotation = FRotator::MakeFromEuler(lookAtFevtor).Quaternion();
		bool bResult = GetWorld()->OverlapMultiByChannel(
			HitResult,
			capsuleLocation,
			capsuleRotation,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeCapsule(_shockWaveRadius, _shockWaveRange),
			SW_HitParams
		);

		if (bResult)
		{
			bool OnSound = false;
			for (auto Hit : HitResult)
			{
				if (Hit.GetActor())
				{
					//LOG_SCREEN("Hit Result: %s", *(Hit.GetActor()->GetName()));
					AHACKED_AI* hackedAI = Cast<AHACKED_AI>(Hit.GetActor());
					if (!hackedAI) continue;

					FPlayerDamageEvent playerEvent;
					playerEvent.bCanChargeUlt = true;
					playerEvent.bCanEsperStack = true;
					playerEvent.bWithAim = true;
					hackedAI->TakeDamage(_shockWaveDamage, playerEvent, GetController(), this);
					FVector pushBack = (hackedAI->GetActorLocation() - GetActorLocation());
					pushBack += FVector::UpVector * 10.0f;
					pushBack.Normalize();
					hackedAI->HACKEDLaunchCharacter(pushBack * 300.0f, 0.05f, 0.025f);
					if (_shockWaveTime <= 2.9f && _shockWaveTime > 2.8f)
					{
						if (!OnSound) {
							PlaySkill2Sound(hackedAI->GetActorLocation());
						}
					}
					if (_shockWaveTime <= 1.9f && _shockWaveTime > 1.8f)
					{
						if (!OnSound) {
							PlaySkill2Sound(hackedAI->GetActorLocation());
						}
					}
					if (_shockWaveTime <= 0.9f && _shockWaveTime > 0.8f)
					{
						if (!OnSound) {
							PlaySkill2Sound(hackedAI->GetActorLocation());
						}
					}
				}
			}
			
		}
		if (_shockWaveTime <= 0.2f && _shockWaveTime > 0.1f)
		{
			FollowCamera->PostProcessSettings.SceneFringeIntensity = 0.0f;
			FollowCamera->PostProcessSettings.BloomIntensity = 1.5f;
			FollowCamera->PostProcessSettings.BloomThreshold = -1.0f;
		}
	}
	else if (_shockWaveTime <= 0.0f)
	{
		CameraBackToOrigin();
		_shockWaveTime = 0.0f;
		GetWorldTimerManager().ClearTimer(ShockWaveTimerHandle);
	}
}

void AHACKED_ESPER::ShockWaveDamagingEnd()
{
	if (ShockWave)
	{
		ShockWave->DestroyComponent();
		ShockWave = nullptr;
	}
	if (ShockWaveHit)
	{
		ShockWaveHit->DestroyComponent();
		ShockWaveHit = nullptr;
	}
}


void AHACKED_ESPER::ShockWaveEnd()
{
	bIsPsychicShockWave = false;
	SetTurnRate();
	if (!bIsSelfHealing)
	{
		SetEnableInput();
	}
}

void AHACKED_ESPER::PsychicShockWaveDamageChange(float plusDamage)
{
	_shockWaveDamage += plusDamage;
}

void AHACKED_ESPER::PsychicShield()
{
	if (NetBaseCP->HasAuthority()) {
		if (!_bCanAttack || OnJump()) return;
		if (!bIsThirdCoolDown || !bCanThirdSkill) return;
		SkillCoolApplySystem(ESkillNumberType::ThirdSkill, false);
		SetEnableInput(false, true, false);

		RPC(NetBaseCP, AHACKED_ESPER, PsychicShield, ENetRPCType::MULTICAST, true);
	}
	bIsPsychicShield = true;
	Esper_Anim->AnimIsPsychicShield = true;
}

void AHACKED_ESPER::RPCCreatePsychicShield(bool isBoth)
{
	RPC(NetBaseCP, AHACKED_ESPER, RPCCreatePsychicShield, ENetRPCType::MULTICAST, true, isBoth);

	if (isBoth) {
		PsychicShieldSpawnEmitter();
		HpComponent->shieldCheck = true;
		HpComponent->SetShieldAmount(_psychicShieldAmount);
		auto hpComponent = HACKEDInGameManager->GetCrusher()->GetHPComponent();
		if (hpComponent)
		{
			HACKEDInGameManager->GetCrusher()->PsychicShieldSpawnEmitter();
			hpComponent->shieldCheck = true;
			hpComponent->SetShieldAmount(_psychicShieldAmount);
		}
	}
	else {
		HpComponent->shieldCheck = true;
		HpComponent->SetShieldAmount(_psychicShieldAmount * 1.5f);
		PsychicShieldSpawnEmitter();
	}
}

void AHACKED_ESPER::PsychicShieldOn()
{
	// 에스퍼 클라이언트에서만 처리한다.
	if (!HasAuthority()) return;

	// 메테리얼 교체 (Shield Material On)
	float playerDistance;
	playerDistance = FVector::Dist(GetActorLocation(), HACKEDInGameManager->GetCrusher()->GetActorLocation());
	LOG_SCREEN("Distance Between Player : %f", playerDistance);
	if (playerDistance > _psychicShieldDistance)
	{
		RPCCreatePsychicShield(false);
	}
	else if (playerDistance < _psychicShieldDistance)
	{
		RPCCreatePsychicShield(true);
	}

	GetWorldTimerManager().SetTimer(PsychicShieldTimerHandle, this, &AHACKED_ESPER::PsychicShieldDestroy, _psychicShieldTime, false);
}

void AHACKED_ESPER::RPCDestroyShield()
{
	RPC(NetBaseCP, AHACKED_ESPER, RPCDestroyShield, ENetRPCType::MULTICAST, true);

	PsychicShieldEndEmitter();
	HACKEDInGameManager->GetCrusher()->PsychicShieldEndEmitter();

	if (HpComponent->GetShieldAmount() > 0.0f)
	{
		HpComponent->SetShieldAmount(0.0f);
	}

	if (HACKEDInGameManager->GetCrusher()->GetHPComponent()->GetShieldAmount() > 0.0f)
	{
		HACKEDInGameManager->GetCrusher()->GetHPComponent()->SetShieldAmount(0.0f);
	}
}

void AHACKED_ESPER::PsychicShieldDestroy()
{
	// 에스퍼 클라이언트에서만 처리한다.
	if (!HasAuthority()) return;

	RPCDestroyShield();
}

void AHACKED_ESPER::PsychicShieldAnimEnd()
{
	if (!bIsSelfHealing)
	{
		SetEnableInput();
	}
}

void AHACKED_ESPER::PsychicShieldTimeChange(float plusTime)
{
	_psychicShieldTime += plusTime;
}

void AHACKED_ESPER::PsychicOverDrive()
{
	if (NetBaseCP->HasAuthority()) {
		if (!_bCanAttack || OnJump()) return;
		if (!bCanUltimate || !bUltimateDisable) return;
		UltimateGaugeReset();
		if(NetBaseCP->HasAuthority()) GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_PsychicOverDrive, 1.0f);
		RPC(NetBaseCP, AHACKED_ESPER, PsychicOverDrive, ENetRPCType::MULTICAST, true);
	}
	UltimateCheckedAI.Empty();
	SetEnableInput(false, true, false);
	bIsPsychicOverDrive = true;
	bOnPsychicOverDriveHovering = true; 
	Esper_Anim->AnimIsPsychicOverDrive = true;
	OverDriveSavePos = GetActorLocation() + FVector(0.0f, 0.0f, 0.1f); // OverDriveSavePos 초기화 
	CameraBoom->SetDesiredArmLength(800.0f, 1.0f);
	CameraBoom->SetDesiredSocketOffset(FVector(0.0f, 0.0f, 90.0f), 1.0f);
	FollowCamera->PostProcessSettings.MotionBlurAmount = 1.0f;
	OnTranslucent(true);
	//UltimateDynamicMaterial->BlendMode = BLEND_Translucent; // ::Translucent;
	//BLEND_Opaque
}


void AHACKED_ESPER::PsychicOverDriveCamReturn()
{
	CameraBoom->SetDesiredArmLength(225.0f, 15.0f);
	CameraBoom->SetDesiredSocketOffset(FVector(-75.0f, 65.0f, 60.0f), 15.0f);
}

void AHACKED_ESPER::PsychicOverDriveEndAction()
{
	bOnPsychicOverDriveHovering = false;
	for (int i = 0; i < UltimateCheckedAI.Num(); ++i) {
		AHACKED_AI* hackedAI = UltimateCheckedAI[i];

		if (hackedAI->IsDie()) {
			UltimateCheckedAI.Remove(hackedAI);
			--i;
			continue;
		}

		FVector savePos = hackedAI->GetActorLocation();
		FVector setPos = savePos;
		setPos.Z -= 300.0f;
		FVector desiredPos = setPos - savePos;
		desiredPos.Normalize();
		hackedAI->LaunchCharacter(desiredPos * 3000.0f, false, true);
		hackedAI->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
		hackedAI->NetBaseCP->PlayStopSyncTime(_ultimateDropDelay);
	}
	UltimateDecal();
	//DC_EsperUltimateDecal
	FVector DecalSpawnPos = GetActorLocation();
	DecalSpawnPos.Z -= 100.0f;
	DecalSpawnPos.X += 150.0f;
	//UGameplayStatics::SpawnDecalAttached(DC_EsperUltimateDecal, FVector(350.0f,350.0f,350.0f), GetCapsuleComponent(), "Bip001-Pelvis"),
	FVector DecalLocation = GetMesh()->GetSocketLocation(FName("Bip001-Pelvis"));
	UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DC_EsperUltimateDecal, FVector(350.0f, 350.0f, 350.0f), DecalLocation, FRotator(-90.0f,0.0f,0.0f), 2.0f);
	FollowCamera->PostProcessSettings.MotionBlurAmount = 0.25f;
	if (NetBaseCP->HasAuthority()) GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(CS_PsychicOverDriveEnd, 1.0f);
	GetWorldTimerManager().SetTimer(UltimateTimerHandle, this, &AHACKED_ESPER::PsychicOverDriveDamaging, _ultimateDropDelay + 0.01f, false);
}

void AHACKED_ESPER::PsychicOverDriveDamaging()
{
	NetBaseCP->EnableSync(true);
	OnTranslucent(false);
	LOG(Warning, "%d", UltimateCheckedAI.Num());
	for (int i = 0; i < UltimateCheckedAI.Num(); ++i) 
	{
		AHACKED_AI* Hit = UltimateCheckedAI[i];
		if(!Hit || Hit->IsDie()) continue;
		Hit->NetBaseCP->EnableSync(true);
		AIRestart(Hit);

		// 조작권자만 피해를 입힌다.
		if (NetBaseCP->HasAuthority()) {
			FPlayerDamageEvent damageType;
			Hit->TakeDamage(_psychicOverDriveDamage, damageType, GetController(), this);
		}
	}
}

void AHACKED_ESPER::PsychicOverDriveEnd()
{
	if(!bIsSelfHealing)
	{
		SetEnableInput();
	}
	bIsPsychicOverDrive = false;
	Esper_Anim->AnimIsPsychicOverDrive = false;
}

void AHACKED_ESPER::PsychicDash()
{
	if (NetBaseCP->HasAuthority())
	{
		if (!_bCanAttack || OnJump()) return;
		if (!bIsDashCoolDown || !bCanDashSkill) return;
		SkillCoolApplySystem(ESkillNumberType::DashSkill, false);

		RPC(NetBaseCP, AHACKED_ESPER, PsychicDash, ENetRPCType::MULTICAST, true);
	}

	bIsPsychicDash = true;
	Esper_Anim->AnimIsPsychicDash = true;

	//PC_EsperSelfHeal = UGameplayStatics::SpawnEmitterAttached(PS_EsperSelfHeal, GetMesh(), FName("Bip001_Spine1"), FVector(0.0f, 0.0f, 90.0f),
	//	FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);

	UGameplayStatics::SpawnEmitterAttached(PS_PsychicDash, GetMesh(), FName("Bip001-Spine2"), FVector(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 0.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);

	DashStartPos = GetActorLocation();
	if (!bOnMove) DashDirection = GetActorForwardVector();
	else DashDirection = (GetForwardSpeedRate() * GetActorForwardVector() + 
		GetRightSpeedRate() * GetActorRightVector()).GetUnsafeNormal();
	_psychicDashPower = 2000.0f;

	if (bIsNowOnTutorial) {
		DashCount++;
	}
	
}

void AHACKED_ESPER::PsychicDashing()
{
	if (FVector::Dist(DashStartPos, GetActorLocation()) >= (_psychicDashDistance * 0.1f))
	{
		if (GetVelocity().Size() <= (_psychicDashPower * 0.5f)) {
			PsychicDashFinish();
			GetCharacterMovement()->StopMovementImmediately();
		}
	}

	if (FVector::Dist(DashStartPos, GetActorLocation()) >= _psychicDashDistance)
	{
		PsychicDashFinish();
		GetCharacterMovement()->StopMovementImmediately();
	}
	else
	{
		GetCharacterMovement()->Velocity = (DashDirection * _psychicDashPower);
	}
}

void AHACKED_ESPER::PsychicDashFinish()
{
	RPC(NetBaseCP, AHACKED_ESPER, PsychicDashFinish, ENetRPCType::MULTICAST, true);
	GetCharacterMovement()->StopMovementImmediately();
	bIsPsychicDash = false;
	Esper_Anim->AnimIsPsychicDash = false;
}



void AHACKED_ESPER::RPCSelfHealing()
{
	RPC(NetBaseCP, AHACKED_ESPER, RPCSelfHealing, ENetRPCType::MULTICAST, true);
	LOG_SCREEN("SelfHealingCheck");
	bIsSelfHealing = true;

	// 자가 치유 PP를 설정한다.
	FollowCamera->PostProcessSettings.VignetteIntensity = 1.0;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0.0f, 0.0f, 0.0f, 1.0f);

	if (HACKEDInGameManager->GetCrusher()->IsOnSelfHealing())
	{
		LOG_SCREEN("Both Die State On");
		SetEnableInput(false, false, false);
		RPCBothDieProcess();
		HACKEDInGameManager->GetCrusher()->RPCBothDieProcess();
		BackToLobby();
	}
	else
	{ 
		Esper_Anim->AnimIsEsperSelfHealing = true;
		SetEnableInput(false, true, false);

		if (PC_EsperSelfHeal) PC_EsperSelfHeal->DestroyComponent();
		PC_EsperSelfHeal = UGameplayStatics::SpawnEmitterAttached(PS_EsperSelfHeal, GetMesh(), FName("None"), FVector(20.0f, -35.0f, 0.0f),
			FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);

		GetWorldTimerManager().SetTimer(SelfHealingTimer, this, &AHACKED_ESPER::SelfHealingEnd, selfHealingTime, false);
	}
}


void AHACKED_ESPER::SelfHealingEnd()
{
	// 둘다 죽었을 경우에는 무시한다.
	if (Esper_Anim->AnimIsEsperSelfHealingFail) return;

	if (PC_EsperSelfHeal)
	{
		PC_EsperSelfHeal->DestroyComponent();
		PC_EsperSelfHeal = nullptr;
	}

	LOG_SCREEN("Self Healing Anim Success Finish");
	HpComponent->SetMaxHP(_esperMaxHp);
	Esper_Anim->AnimIsEsperSelfHealing = false;
}


void AHACKED_ESPER::SelfHealingAnimEnd()
{
	if (PC_EsperSelfHeal)
	{
		PC_EsperSelfHeal->DestroyComponent();
		PC_EsperSelfHeal = nullptr;
	}

	// 자가 치유 PP를 초기화 한다.
	FollowCamera->PostProcessSettings.VignetteIntensity = 0.0;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

	LOG_SCREEN("Self Healing End");
	bIsSelfHealing = false;
	SetEnableInput();
}

bool AHACKED_ESPER::IsOnSelfHealing()
{
	return Esper_Anim->AnimIsEsperSelfHealing || Esper_Anim->AnimIsEsperSelfHealingFail;
}

void AHACKED_ESPER::RPCBothDieProcess()
{
	RPC(NetBaseCP, AHACKED_ESPER, RPCBothDieProcess, ENetRPCType::MULTICAST, true);
	if (PC_EsperSelfHeal) {
		PC_EsperSelfHeal->DestroyComponent();
		PC_EsperSelfHeal = nullptr;
	}
	Esper_Anim->AnimIsEsperSelfHealingFail = true;
	Esper_Anim->AnimIsEsperDieProcess = true;
	DisableAimWidget();
}

