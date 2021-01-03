// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "HACKEDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "InGame/Network/Component/NetworkTransformCP.h"
#include "InGame/Character/HACKED_CameraArmComponent.h"
#include "NetworkModule/NetworkData.h"
#include "HACKED_SkillSystemComponent.h"
#include "../Stat/HpComponent.h"
#include "InGame/Character/Esper/HACKED_Esper.h"
#include "../Phase/HackingPanel.h"
#include "InGame/HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"


// 두 float의 차이를 계산하는 함수.
float Gab(float a, float b) {
	a = FMath::Abs(a);
	b = FMath::Abs(b);
	if (a > b) return a - b;
	else return b - a;
}

void AHACKEDCharacter::SetEnableInput(bool canMove, bool canTurn, bool canAttack)
{
	_bCanMove = canMove;
	_bCanTurn = canTurn;
	_bCanAttack = canAttack;
}

void AHACKEDCharacter::SetTurnRate(float baseTurnRate /*= 20.0f*/, float baseLookUpRate /*= 20.0f*/)
{
	BaseTurnRate = baseTurnRate;
	BaseLookUpRate = baseLookUpRate;
}

void AHACKEDCharacter::ShowInfoWidget()
{
	if (!WG_Info) WG_Info = CreateWidget<
	>(GetWorld(), WG_Info_Class);
	if (!WG_Info->IsInViewport()) WG_Info->AddToViewport();
}

void AHACKEDCharacter::CloseInfoWidget()
{
	WG_Info->RemoveFromViewport();
}

float AHACKEDCharacter::GetSelfHealingRate()
{
	if (bIsSelfHealing)
		return 1.0f - (GetWorld()->GetTimerManager().GetTimerRemaining(SelfHealingTimer) / selfHealingTime);
	else
		return 0.0f;
}

void AHACKEDCharacter::SkillDisable(bool canPrimary, bool canM2, bool canFirst, bool canSecond, bool canThird, bool canDash, bool canUltimate)
{
	bCanPrimary = canPrimary;
	bCanM2Skill = canM2;
	bCanFirstSkill = canFirst;
	bCanSecondSkill = canSecond;
	bCanThirdSkill = canThird; 
	bCanDashSkill = canDash;
	bUltimateDisable = canUltimate;
}

AHACKEDCharacter::AHACKEDCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer) 
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	HpComponent = CreateDefaultSubobject<UHpComponent>(TEXT("HpComponent"));
	SpCost_Stat = CreateDefaultSubobject<UHACKED_SkillSystemComponent>(TEXT("SpSytem Stat CP"));

	// set our turn rates for input
	BaseTurnRate = 20.f;
	BaseLookUpRate = 20.f;

	// 콜리전 프로파일 설정 
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("HACKED_CHARACTER"));

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->SetJumpAllowed(false);	//점프 불가능
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->SetWalkableFloorAngle(60.0f);


	GetCharacterMovement()->BrakingDecelerationWalking = 50.0f;
	//;GetCharacterMovement()->

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<UHACKED_CameraArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 225.0f; // The camera follows at this distance behind the character	
	CameraBoom->SetDesiredArmLength(225.0f);
	CameraBoom->SocketOffset = FVector(-14.0f, 90.0f, 55.0f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
	NetTransformCP = CreateDefaultSubobject<UNetworkTransformCP>(TEXT("Network Transform"));

	// Setting syncvars
	_currentSpeed.SetReliable(false);
	_currentForwardSpeedRate.SetReliable(false);
	_currentRightSpeedRate.SetReliable(false);

	// Disable UE4 replication
	bReplicates = false;
	SetReplicateMovement(false);

	// Init SyncVars
	_bOnJump = false;
	_currentSpeed = 0.f;
	_currentForwardSpeedRate = 0.f;
	_currentRightSpeedRate = 0.f;

	_bOnJump.Init(&NetBaseCP, TEXT("bOnJump"));
	_bOnJump.SetReliable(true);

	_currentSpeed.Init(&NetBaseCP, TEXT("currentSpeed"));
	_currentSpeed.SetReliable(false);

	_currentForwardSpeedRate.Init(&NetBaseCP, TEXT("currentForwardSpeedRate"));
	_currentForwardSpeedRate.SetReliable(false);

	_currentRightSpeedRate.Init(&NetBaseCP, TEXT("currentRightSpeedRate"));
	_currentRightSpeedRate.SetReliable(false);

	NetTransformCP->teleportLimit = 3000.0f;

	// Psychic Shield
	//static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_PSYCHICSHIELD(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/Esper_03/Esper_03_Effect_Loop.Esper_03_Effect_Loop"));
	//{
	//	PS_OnPsychicShield = PS_PSYCHICSHIELD.Object;
	//}

	//GetObjectAsset(PS_PsychicShieldStart, UParticleSystem, "/Game/Resources/Character/Esper/FX/NEW_v2/Esper_03/Esper_03_Effect_Activated.Esper_03_Effect_Activated");

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_PSYCHICSHIELDEND(TEXT("/Game/Resources/Character/Esper/FX/NEW_v2/Esper_03/Esper_03_Effect_Deactivate.Esper_03_Effect_Deactivate"));
	//{
	//	PS_PsychicShieldEnd = PS_PSYCHICSHIELDEND.Object;
	//}

	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_PSYCHICSHIELD(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P_Buff_Shield.P_Buff_Shield"));
	{
		PS_OnPsychicShield = PS_PSYCHICSHIELD.Object;
	}

	GetObjectAsset(PS_PsychicShieldStart, UParticleSystem, "/Game/Resources/Character/Esper/FX/NEW_v2/Esper_03/Esper_03_Effect_Activated.Esper_03_Effect_Activated");

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_PSYCHICSHIELDEND(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P_Buff_Shield_broken.P_Buff_Shield_broken"));
	{
		PS_PsychicShieldEnd = PS_PSYCHICSHIELDEND.Object;
	}

	// Vitality Shield
	//GetObjectAsset(PS_VitalityShield, UParticleSystem, "/Game/Resources/Character/Crusher/FX/V_Shield/P_Crusher_03.P_Crusher_03");
	GetObjectAsset(PS_VitalityShield, UParticleSystem, "/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P_Buff_Shield_ChangeHP.P_Buff_Shield_ChangeHP");
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_VITALITYSHIELDEND(TEXT("/Game/Resources/Character/Crusher/FX/V_Shield/P_Crusher_03_End.P_Crusher_03_End"));
	{
		PS_VitalityShieldEnd = PS_VITALITYSHIELDEND.Object;
	}





	// Adrenal Surge
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_ESPERADRENALSURGE(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P_Aura_E.P_Aura_E"));
	{
		PS_EsperAdrenalSurge= PS_ESPERADRENALSURGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_CRUSHERADRENALSURGE(TEXT("/Game/Resources/Character/FX_Ver_2/FX2_Particle/Shield/P_Aura_C.P_Aura_C"));
	{
		PS_CrusherAdrenalSurge = PS_CRUSHERADRENALSURGE.Object;
	}

	// Init SpSystem
	MaxSp = 24;
	SpSytemSetUpState();

	// UltimateSystem
	currentUltimateGauge = 0.0f;
	maxUltimateGauge = 1000.0f;

	bCanUltimate = false;
	
	_ultimateTime = 0.0f;

	// Self Healing
	bIsSelfHealing = false; 
	selfHealingTime = 10.0f;

	GetCapsuleComponent()->SetUseCCD(true);
	SetEnableInput();
	
	//HACKING PANEL
	currentHackingPanel = nullptr;
}

void AHACKEDCharacter::Possessed(AController* NewController)
{
	if (!HasActorBegunPlay() || !HACKEDInGameManager) return;
	if(HasAuthority()) HACKEDInGameManager->GetFadeInOutMaker()->FadeIn(1.0f);
}

bool AHACKEDCharacter::HasAuthority() {
	if (!NetBaseCP) return true;
	return NetBaseCP->HasAuthority();
}

float AHACKEDCharacter::CurrentIntroTime()
{
	return currentIntroSkipTime / maxtIntroSkipTime;
}

bool AHACKEDCharacter::GetWantToSkipTuto()
{
	return bSkipTutorial;
}

bool AHACKEDCharacter::GetSkipTutoCallCheck()
{
	return bSkipFunctionUsedCheck;
}

void AHACKEDCharacter::WantToSkipTuto(bool skipCheck, bool firstCheck)
{

	RPC(NetBaseCP, AHACKEDCharacter, WantToSkipTuto, ENetRPCType::MULTICAST, true, skipCheck, firstCheck);

	bSkipFunctionUsedCheck = firstCheck;

	if (skipCheck)
		bSkipTutorial = true;
	else
		bSkipTutorial = false;
}

float AHACKEDCharacter::GetForwardSpeedRate()
{
	return *_currentForwardSpeedRate;
}
float AHACKEDCharacter::GetRightSpeedRate()
{
	return *_currentRightSpeedRate;
}

float AHACKEDCharacter::GetCurrentUltimateGauge()
{
	return currentUltimateGauge;
}

void AHACKEDCharacter::UltimateGauageSystem()
{
	if (currentUltimateGauge < maxUltimateGauge)
		bCanUltimate = false;
	else
	{
		bCanUltimate = true;
	}
}

void AHACKEDCharacter::UltimateGaugeReset()
{
	bCanUltimate = false;
	currentUltimateGauge = 0.0f;
}

void AHACKEDCharacter::InitSpCostStatByType(const EHackedHeroType& type)
{
	if (!SpCost_Stat) {
		LOG_ERROR("SpCost_Stat not exist..");
		return;
	}
	spStat = SpCost_Stat->GetSpCostStat(type);
}

int AHACKEDCharacter::GetM2SkillCost()
{
	if (!spStat) return 0;
	return spStat->m2SkillTimeCost;
}

int AHACKEDCharacter::GetFirstSkillCost()
{
	if (!spStat) return 0;
	return spStat->firstSkillTimeCost;
}

int AHACKEDCharacter::GetSecondSkillCost()
{
	if (!spStat) return 0;
	return spStat->secondSkillTimeCost;
}

int AHACKEDCharacter::GetThirdSkillCost()
{
	if (!spStat) return 0;
	return spStat->thirdSkillTimeCost;
}

int AHACKEDCharacter::GetDashSkillCost()
{
	if (!spStat) return 0;
	return spStat->dashSkillTimeCost;
}

float AHACKEDCharacter::GetUltimateMaxGauge()
{
	if (!spStat) return 0;
	return spStat->UltimateGaugeCost;
}

int AHACKEDCharacter::GetM2SkillRate()
{
	return (int)_m2SkillTime;
}

int AHACKEDCharacter::GetFirstSkillRate()
{
	return (int)_firstSkillTime;
}

int AHACKEDCharacter::GetSecondSkillRate()
{
	return (int)_secondSkillTime;
}

int AHACKEDCharacter::GetThirdSkillRate()
{
	return (int)_thirdSkillTime;
}

int AHACKEDCharacter::GetDashSkillRate()
{
	return (int)_dashSkillTime;
}

void AHACKEDCharacter::SpSytemSetUpState()
{
	CurrentSp = 0;
	LOG(Warning, "SpSytemSetUpState Current Sp : %d", CurrentSp);
}

void AHACKEDCharacter::SpSystemStartState()
{
	CHECK(FMath::IsWithinInclusive<int32>(CurrentSp, 0, MaxSp - 1));
	CurrentSp = FMath::Clamp<int32>(CurrentSp + 1, 0, MaxSp);
	LOG(Warning, "Current Sp : %d", CurrentSp);
	_RPCSaveSP(CurrentSp);
}

bool AHACKEDCharacter::SpReduceApplyState(const ESkillNumberType & skillNum)
{
	if (bWithoutSP) return true;

	int amount = 0;

	switch (skillNum)
	{
	case ESkillNumberType::M2Skill:
		amount = GetM2SkillCost();
		break;
	case ESkillNumberType::FirstSkill:
		amount = GetFirstSkillCost();
		break;
	case ESkillNumberType::SecondSkill:
		amount = GetSecondSkillCost();
		break;
	case ESkillNumberType::ThirdSkill:
		amount = GetThirdSkillCost();
		break;
	default:
		LOG_S(Error);
		break;
	}

	if (CurrentSp >= amount)
	{
		CurrentSp -= amount;
		_RPCSaveSP(CurrentSp);
		LOG(Warning, "After Skill Current Sp : %d", CurrentSp);
		return true;
	}
	else
	{
		LOG(Warning, "Can't use skill : %d", CurrentSp);
		return false;
	}

}

void AHACKEDCharacter::SkillCoolApplySystem(const ESkillNumberType& skillNum, bool canSkill = false)
{
	if (canSkill)
	{
		switch (skillNum)
		{
		case ESkillNumberType::M2Skill:
			bIsM2CoolDown = true;
			break;
		case ESkillNumberType::FirstSkill:
			bIsFirstCoolDown = true;
			break;
		case ESkillNumberType::SecondSkill:
			bIsSecondCoolDown = true;
			break;
		case ESkillNumberType::ThirdSkill:
			bIsThirdCoolDown = true;
			break;
		case ESkillNumberType::DashSkill:
			bIsDashCoolDown = true;
			break;
		default:
			LOG_S(Error);
			break;
		}
	}

	else if (!canSkill)
	{

		switch (skillNum)
		{
		case ESkillNumberType::M2Skill:
			bIsM2CoolDown = false;
			_m2SkillTime = GetM2SkillCost();
			GetWorldTimerManager().SetTimer(M2SkillTimerHandle, this, &AHACKEDCharacter::M2SkillTimerCheck, 1.0f, true);
			break;
		case ESkillNumberType::FirstSkill:

			bIsFirstCoolDown = false;
			_firstSkillTime = GetFirstSkillCost();
			GetWorldTimerManager().SetTimer(FirstSkillTimerHandle, this, &AHACKEDCharacter::FirstSkillTimerCheck, 1.0f, true);
			break;
		case ESkillNumberType::SecondSkill:
			bIsSecondCoolDown = false;
			_secondSkillTime = GetSecondSkillCost();
			GetWorldTimerManager().SetTimer(SecondSkillTimerHandle, this, &AHACKEDCharacter::SecondSkillTimerCheck, 1.0f, true);
			break;
		case ESkillNumberType::ThirdSkill:
			bIsThirdCoolDown = false;
			_thirdSkillTime = GetThirdSkillCost();
			GetWorldTimerManager().SetTimer(ThirdSkillTimerHandle, this, &AHACKEDCharacter::ThirdSkillTimerCheck, 1.0f, true);
			break;
		case ESkillNumberType::DashSkill:
			bIsDashCoolDown = false;
			_dashSkillTime = GetDashSkillCost();
			GetWorldTimerManager().SetTimer(DashSkillTimerHandle, this, &AHACKEDCharacter::DashSkillTimerCheck, 1.0f, true);

		default:
			LOG_S(Error);
			break;
		}
	}
}

void AHACKEDCharacter::M2SkillTimerCheck()
{
	_m2SkillTime -= 1.0f;
	if (_m2SkillTime <= 0)
	{
		bIsM2CoolDown = true;
		LOG_SCREEN("First Skill Cool Down");
		GetWorldTimerManager().ClearTimer(M2SkillTimerHandle);
	}
}

void AHACKEDCharacter::FirstSkillTimerCheck()
{
	_firstSkillTime -= 1.0f;
	if (_firstSkillTime <= 0)
	{
		bIsFirstCoolDown = true;
		LOG_SCREEN("Second Skill Cool Down");
		GetWorldTimerManager().ClearTimer(FirstSkillTimerHandle);
	}

}

void AHACKEDCharacter::SecondSkillTimerCheck()
{
	_secondSkillTime -= 1.0f;
	if (_secondSkillTime <= 0)
	{
		bIsSecondCoolDown = true;
		LOG_SCREEN("Third Skill Cool Down");
		GetWorldTimerManager().ClearTimer(SecondSkillTimerHandle);
	}
}

void AHACKEDCharacter::ThirdSkillTimerCheck()
{
	_thirdSkillTime -= 1.0f;
	if (_thirdSkillTime <= 0)
	{
		bIsThirdCoolDown = true;
		LOG_SCREEN("Fourth Skill Cool Down");
		GetWorldTimerManager().ClearTimer(ThirdSkillTimerHandle);
	}
}

void AHACKEDCharacter::DashSkillTimerCheck()
{
	_dashSkillTime -= 1.0f;
	if (_dashSkillTime <= 0)
	{
		bIsDashCoolDown = true;
		LOG_SCREEN("Dash Skill Cool Down");
		GetWorldTimerManager().ClearTimer(DashSkillTimerHandle);
	}
}

// 에스퍼와 크러셔에 맞는 ultimateTime 입력 
void AHACKEDCharacter::UltimateApplyState(float ultimateTime, bool canUltimate /*= false*/)
{
	if (canUltimate)
	{
		bCanUltimate = true;
	}
	else if (!canUltimate)
	{
		bCanUltimate = false;
		_maxUltimateTime = ultimateTime;
		_ultimateTime = 0.0f;
		GetWorldTimerManager().SetTimer(UltimateTimerCheckHandle, this, &AHACKEDCharacter::UltimateTimerCheck, 0.1f, true);
	}
}

void AHACKEDCharacter::UltimateTimerCheck()
{
	_ultimateTime = _ultimateTime + 0.1f;
	if (_ultimateTime >= _maxUltimateTime)
	{
		bCanUltimate = true;
		GetWorldTimerManager().ClearTimer(UltimateTimerCheckHandle);
	}
}


float AHACKEDCharacter::GetSPRate()
{
	return (float)CurrentSp;
}

int32 AHACKEDCharacter::GetSp()
{
	return CurrentSp;
}

float AHACKEDCharacter::GetHpRate()
{
	return HpComponent->GetHP() / HpComponent->GetMaxHp();
}

float AHACKEDCharacter::GetShieldRate()
{
	return HpComponent->GetShieldAmount() / HpComponent->GetMaxShield();
}

float AHACKEDCharacter::GetUltimateRate()
{
	if (currentUltimateGauge > maxUltimateGauge)
	{
		return 1.0f;
	}
	return currentUltimateGauge / maxUltimateGauge;
}

void AHACKEDCharacter::RPCSetHPAndShield(float hp, float shield)
{
	RPC(NetBaseCP, AHACKEDCharacter, RPCSetHPAndShield, ENetRPCType::MULTICAST, true, hp, shield);

	if (NetBaseCP->HasAuthority()) return;
	HpComponent->SetHPAndShield(hp, shield);
}

void AHACKEDCharacter::_RPCSaveSP(int32 sp)
{
	RPC(NetBaseCP, AHACKEDCharacter, _RPCSaveSP, ENetRPCType::MULTICAST, true, sp);
	_savedSP = sp;
}

void AHACKEDCharacter::_RPCRefreshSP(int32 sp)
{
	RPC(NetBaseCP, AHACKEDCharacter, _RPCRefreshSP, ENetRPCType::MULTICAST, true, sp);
	CurrentSp = sp;
	_savedSP = sp;
}

void AHACKEDCharacter::PsychicShieldSpawnEmitter()
{
	//PC_OnPsychicShield->Activate();
	auto PSPsychicShieldStart = UGameplayStatics::SpawnEmitterAttached(PS_PsychicShieldStart, GetMesh(), FName("None"), FVector(0.0f, 0.0f, 110.0f),
		FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);

	if (PC_OnPsychicShield) {
		PC_OnPsychicShield->DestroyComponent();
		PC_OnPsychicShield = nullptr;
	}
	PC_OnPsychicShield = UGameplayStatics::SpawnEmitterAttached(PS_OnPsychicShield, GetMesh(), FName("None"), FVector(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);

	//PSPsychicShieldStart->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
}

void AHACKEDCharacter::PsychicShieldEndEmitter()
{
	RPC(NetBaseCP, AHACKEDCharacter, PsychicShieldEndEmitter, ENetRPCType::MULTICAST, true);

	if (PC_OnPsychicShield) 
	{
		PC_OnPsychicShield->DestroyComponent();
		PC_OnPsychicShield = nullptr;
		auto PSPsychicShieldEnd = UGameplayStatics::SpawnEmitterAttached(PS_PsychicShieldEnd, GetMesh(), FName("None"), FVector(0.0f, 0.0f, 0.0f),
			FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);
	}

	//PSPsychicShieldEnd->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
}

void AHACKEDCharacter::VitalityShieldSpawnEmitter()
{
	if (PC_OnPsychicShield)
	{
		PC_OnPsychicShield->DestroyComponent();
		PC_OnPsychicShield = nullptr;
		//auto PSPsychicShieldEnd = UGameplayStatics::SpawnEmitterAttached(PS_PsychicShieldEnd, GetMesh(), FName("None"), FVector(0.0f, 0.0f, 0.0f),
			//FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);
	}

	auto PSVitalityShield = UGameplayStatics::SpawnEmitterAttached(PS_VitalityShield, GetMesh(), FName("None"), FVector(0.0f, 0.0f, 0.0f),
		FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);

	//PSVitalityShield->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
}

void AHACKEDCharacter::VitalityShieldEndEmitter()
{
	UGameplayStatics::SpawnEmitterAttached(PS_VitalityShieldEnd, GetMesh(), FName("None"), FVector(0.0f, 0.0f, 110.0f),
		FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, true);
}

void AHACKEDCharacter::ShowGameOver()
{
	HACKEDInGameManager->GetFadeInOutMaker()->ShowGameOver();
}

void AHACKEDCharacter::BackToLobby()
{
	GetWorld()->GetTimerManager().SetTimer(GameOverTimer, this, &AHACKEDCharacter::ShowGameOver, _gameOverFadeInTime, false);
	GetWorld()->GetTimerManager().SetTimer(BackToLobbyTimer, this, &AHACKEDCharacter::OnBackToLobby, _gameOverFadeInTime + _gameOverFadeOutTime, false);
}

void AHACKEDCharacter::OnBackToLobby()
{
	HACKEDInGameManager->GetNetworkProcessor()->EndGame();
}

void AHACKEDCharacter::AdrenalSurgeSpawnEmitter(bool selfCheck)
{
	if (selfCheck)
	{
		if (PC_CrusherAdrenalSurge)
		{
			PC_CrusherAdrenalSurge->DestroyComponent();
			PC_CrusherAdrenalSurge = nullptr;
		}

		PC_CrusherAdrenalSurge = UGameplayStatics::SpawnEmitterAttached(PS_CrusherAdrenalSurge, GetCapsuleComponent(), FName("None"), FVector(0.0f, 0.0f, 50.0f),
			FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);

		PC_CrusherAdrenalSurge->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	}
	else if (!selfCheck)
	{
		if (PC_EsperAdrenalSurge)
		{
			PC_EsperAdrenalSurge->DestroyComponent();
			PC_EsperAdrenalSurge = nullptr;
		}

		PC_EsperAdrenalSurge = UGameplayStatics::SpawnEmitterAttached(PS_EsperAdrenalSurge, GetCapsuleComponent(), FName("None"), FVector(0.0f, 0.0f, 50.0f),
			FRotator(0.0f, 90.0f, 0.0f), EAttachLocation::KeepRelativeOffset, false);

		PC_EsperAdrenalSurge->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	}
}

void AHACKEDCharacter::AdrenalSurgeEndEmitter()
{
	if (PC_EsperAdrenalSurge)
	{
		PC_EsperAdrenalSurge->DestroyComponent();
		PC_EsperAdrenalSurge = nullptr;
	}

	if (PC_CrusherAdrenalSurge)
	{
		PC_CrusherAdrenalSurge->DestroyComponent();
		PC_CrusherAdrenalSurge = nullptr;
	}
}

void AHACKEDCharacter::Reconnected()
{
	_RPCRefreshSP(_savedSP);
}

void AHACKEDCharacter::BeginPlay()
{
	Super::BeginPlay();

	BindHACKEDInGameManager(HACKEDInGameManager);
	BindRPCFunction(NetBaseCP, AHACKEDCharacter, RPCSetHPAndShield);
	BindRPCFunction(NetBaseCP, AHACKEDCharacter, _RPCSaveSP); 
	BindRPCFunction(NetBaseCP, AHACKEDCharacter, _RPCRefreshSP);
	BindRPCFunction(NetBaseCP, AHACKEDCharacter, PsychicShieldEndEmitter);
	BindRPCFunction(NetBaseCP, AHACKEDCharacter, WantToSkipTuto);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AHACKEDCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	/*PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHACKEDCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AHACKEDCharacter::StopJumping);*/
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AHACKEDCharacter::PressHackingPanel);

	PlayerInputComponent->BindAction("Information", IE_Pressed, this, &AHACKEDCharacter::ShowInfoWidget);
	PlayerInputComponent->BindAction("Information", IE_Released, this, &AHACKEDCharacter::CloseInfoWidget);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHACKEDCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHACKEDCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AHACKEDCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AHACKEDCharacter::LookUpAtRate);
}

void AHACKEDCharacter::Jump()
{
	if (!_bCanMove) return;
	ACharacter::Jump();
}

void AHACKEDCharacter::StopJumping()
{
	ACharacter::StopJumping();
}

void AHACKEDCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CameraBoom->SetSpringArmComponent();
	HpComponent->OnNoShield.AddUObject(this, &AHACKEDCharacter::PsychicShieldEndEmitter);
	
	//CameraBoom->SetDesiredSocketOffset(FVector(-14.0f, 90.0f, 55.0f));
}

void AHACKEDCharacter::TurnAtRate(float Rate)
{
	if (!_bCanTurn) return;
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHACKEDCharacter::LookUpAtRate(float Rate)
{
	if (!_bCanTurn) return;
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHACKEDCharacter::MoveForward(float Value)
{
	if (!_bCanMove) return;
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHACKEDCharacter::MoveRight(float Value)
{
	if (!_bCanMove) return;
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AHACKEDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// refresh movement state vars (onJump, _currentSpeed)
	CHECK(NetBaseCP);
	if (NetBaseCP->HasAuthority()) {
		// onJump
		if (!_bOnJump && GetMovementComponent()->IsFalling()) {
			_bOnJump = true;
		}
		else if (_bOnJump && !GetMovementComponent()->IsFalling()) {
			_bOnJump = false;
		}

		// _currentSpeed
		float velocityLength = GetVelocity().Size();
		if (Gab(_currentSpeed, velocityLength) >= 0.01f) {
			_currentSpeed = velocityLength;
		}

		// get current forward/right speed rate
		FVector controlRotationVector = GetControlRotation().Euler();
		controlRotationVector.X = 0.f;
		controlRotationVector.Y = 0.f;

		FRotator axisRotator = FRotator::MakeFromEuler(controlRotationVector);
		axisRotator = axisRotator.GetInverse();

		FVector velocityVector = GetCharacterMovement()->Velocity;
		velocityVector.Z = 0.f;
		velocityVector.Normalize();
		FVector relateMoveVector = axisRotator.RotateVector(velocityVector);

		_currentForwardSpeedRate = relateMoveVector.X;
		_currentRightSpeedRate = relateMoveVector.Y;
	}

	currentSpeedRate = _currentSpeed / GetCharacterMovement()->GetMaxSpeed();
	if (currentSpeedRate >= 0.1f) bOnMove = true;
	else bOnMove = false;
}

bool AHACKEDCharacter::OnJump() {
	return *_bOnJump;
}

float AHACKEDCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (NetBaseCP->OnServer() && !NetBaseCP->HasAuthority()) return HpComponent->GetHP();

	float hp = HpComponent->GetHP();

	if (bIsImmortal)
	{
		return hp;
	}
	else if (!bIsSelfHealing)
	{
		//LOG_SCREEN("bIsImmortal : %d", bIsImmortal);
		hp = HpComponent->TakeDamage(DamageAmount);
		PlayHitSound();
		OnDamaged();
		RPCSetHPAndShield(HpComponent->GetHP(), HpComponent->GetShieldAmount());
	}		
	return hp;
}

void AHACKEDCharacter::PressHackingPanel()
{
	if (currentHackingPanel == nullptr)
		return;
	else
	{
		currentHackingPanel->StartHacking();
	}
}

void AHACKEDCharacter::SetHackingPanel(AHackingPanel* hackingPanel)
{
	currentHackingPanel = hackingPanel;

	if (currentHackingPanel != nullptr)
		LOG_SCREEN("%s",*currentHackingPanel->GetName());
}
