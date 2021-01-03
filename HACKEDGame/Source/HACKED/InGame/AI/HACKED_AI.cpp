// Fill out your copyright notice in the Description page of Project Settings.

#include "HACKED_AI.h"
#include "../Stat/HpComponent.h"
#include "AiController.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "InGame/HACKEDTool.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "NetworkModule/Serializer.h"
#include "InGame/Stat/PlayerDamageEvent.h"
#include "InGame/Spawner/Spawner.h"
#include "Core/HACKEDGameInstance.h"
#include "InGame/UI/WGDamageOutput.h"

using namespace JHNETSerializer;
using namespace AIAllStopTool;

//void AHACKED_AI::Possessed(AController* NewController)
//{
//	if (Cast<APlayerController>(NewController) && NetBaseCP->HasAuthority())
//	{
//		
//	}
//}

// Sets default values
AHACKED_AI::AHACKED_AI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
	HpComponent = CreateDefaultSubobject<UHpComponent>(TEXT("HpComponent"));
	AI_Stat = CreateDefaultSubobject<UAIStatComponent>(TEXT("AI Stat CP"));
	HackedAIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HACKED_AI_HPBARWIDGET"));

	HackedAIWidget->SetupAttachment(GetMesh());

	GetCharacterMovement()->SetWalkableFloorAngle(30.0f);
	bReplicates = false;
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("HACKED_AI"));
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	_bDie = false;
	_bOnAI = true;
	bPatrolCheck = false;

	HackedAIWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
	HackedAIWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> hackedAIHpBar(TEXT("/Game/Blueprint/HACKED_AI_UI/HACKED_AI_UI.HACKED_AI_UI_C"));
	if (hackedAIHpBar.Succeeded())
	{
		HackedAIWidget->SetWidgetClass(hackedAIHpBar.Class);
		HackedAIWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> AI_DIEEXPLOSION(TEXT("/Game/Resources/Enemy/AI/P_AI_Explod.P_AI_Explod"));
	{
		PS_DieExplosion = AI_DIEEXPLOSION.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> AI_BarrierHit(TEXT("/Game/Resources/Character/Crusher/P_Swall_Hit.P_Swall_Hit"));
	{
		PS_BarrierHit = AI_BarrierHit.Object;
	}
	// Stack
	currentStack = 0.0f;
	_rigityCoolTime = 8.0f;


#ifdef ENABLE_TRANSFORM_SYNC
	NetTransformCP = CreateDefaultSubobject<UNetworkTransformCP>(TEXT("Network Transform"));
	NetTransformCP->sendRate = 0.2f;
	NetTransformCP->interporationSpeed = 0.2f;
	NetTransformCP->teleportLimit = 700.0f;
	NetTransformCP->sendLocationGab = 10.0f;
	NetTransformCP->sendRotationGab = 10.0f;
#endif


#ifdef DEBUG_NETWORK_ID
	NetIDViewer = CreateDefaultSubobject<UNetIDViewer>(TEXT("IDViewer"));
#endif

	_meshOriginTransform = FTransform::Identity;



}

void AHACKED_AI::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsDie()) {
		if (HackedAIWidget->IsVisible()) HackedAIWidget->SetVisibility(false);
	}
	else 
	{
		if (HpComponent->GetHP() < HpComponent->GetMaxHp())
		{
			if (!HackedAIWidget->IsVisible()) HackedAIWidget->SetVisibility(true);
			char hpFuncCallBuf[30];
			char stackFuncCallBuf[30];
			_snprintf_s(hpFuncCallBuf, sizeof(hpFuncCallBuf), "SetHpRate %f", GetAIHP());
			_snprintf_s(stackFuncCallBuf, sizeof(stackFuncCallBuf), "SetStackRate %f", GetAIStack());
			FOutputDeviceNull ar;
			HackedAIWidget->GetUserWidgetObject()->CallFunctionByNameWithArguments(ANSI_TO_TCHAR(hpFuncCallBuf), ar, NULL, true);
			HackedAIWidget->GetUserWidgetObject()->CallFunctionByNameWithArguments(ANSI_TO_TCHAR(stackFuncCallBuf), ar, NULL, true);
		}
		else
		{
			if (HackedAIWidget->IsVisible()) HackedAIWidget->SetVisibility(false);
		}
	}

	if (_onLaunch) {
		FHitResult result;
		FVector moveVec = _launchVelocity * DeltaSeconds * _launchSlowTimer;
		SetActorLocation(GetActorLocation() + moveVec, true, &result);

		_launchTime -= DeltaSeconds;
		if (_launchTime <= 0.0f) 
		{
			AIAllStopTool::AIRestart(this);
			_onLaunch = false;
		}
		else if (_launchSlowStartTime >= 0 && _launchTime <= _launchSlowStartTime) 
		{
			_launchSlowTimer = FMath::Clamp(_launchSlowTimer - (DeltaSeconds / _launchSlowStartTime), 0.0f, 1.0f);
		}
	}
}

TArray<AHACKEDCharacter*> AHACKED_AI::GetPlayers()
{
	if (_players.Num() <= 0) {
		RefreshPlayers();
	}
	return _players;
}

void AHACKED_AI::OnEnableAI(bool isEnable)
{
	if (NetBaseCP->IsMaster()) {
		_bOnAI = isEnable;
	}
	if (!isEnable) {
		GetCharacterMovement()->StopActiveMovement();
		SetActorLocation(GetActorLocation() + GetActorUpVector(), false, nullptr, ETeleportType::ResetPhysics);
	}
}

void AHACKED_AI::RPCBeHitAnimation()
{
	RPC(NetBaseCP, AHACKED_AI, RPCBeHitAnimation, ENetRPCType::MULTICAST, true);
	BeHitAnimation();
}


void AHACKED_AI::BeHitAnimation()
{
	/*if (!bCanBossBeHit) return;*/
	AIAllStopTool::AIDisable(this);
	bCanBeHitAnimation = false;
	RPCBeHitAnimation();
}

void AHACKED_AI::OnEndHitAnimation()
{
	AIAllStopTool::AIRestart(this);
	bCanBeHitAnimation = true;
}

void AHACKED_AI::RigidityAI()
{
	if (!bCanRigidity) return;
	/*if (!bCanBossRigidity) return;*/

	AIAllStopTool::AIDisable(this);
	bCanRigidity = false;
	GetWorldTimerManager().SetTimer(RigidityHandle, this, &AHACKED_AI::RigidityCoolDown, _rigityCoolTime, false);

	char stunCallBuf[30];
	_snprintf_s(stunCallBuf, sizeof(stunCallBuf), "StartCrusherStun %f", _rigityCoolTime);
	FOutputDeviceNull ar;
	HackedAIWidget->GetUserWidgetObject()->CallFunctionByNameWithArguments(ANSI_TO_TCHAR(stunCallBuf), ar, NULL, true);
	RPCRigidityAI();
}

void AHACKED_AI::RPCRigidityAI()
{
	RPC(NetBaseCP, AHACKED_AI, RPCRigidityAI, ENetRPCType::MULTICAST, true);
	RigidityAI();
}

void AHACKED_AI::RigidityEnd()
{
	LOG_SCREEN("HACKED AI RigidityEndCheck");
	AIAllStopTool::AIRestart(this);
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
}

void AHACKED_AI::RigidityCoolDown()
{
	bCanRigidity = true;
}

void AHACKED_AI::HACKEDLaunchCharacter(FVector velocity, float time, float slowStartTime)
{
	if (velocity.Equals(FVector::ZeroVector)) 
	{
		_onLaunch = false;
	}
	else 
	{
		_launchVelocity = velocity;
		_onLaunch = true;
		_launchTime = time;
		_launchSlowStartTime = slowStartTime;
		_launchSlowTimer = 1.0f;
		AIAllStopTool::AIDisable(this);
	}
}

float AHACKED_AI::GetAIHP()
{
	return HpComponent->GetHP() / HpComponent->GetMaxHp();
}

float AHACKED_AI::GetAIStack()
{
	return currentStack;
}

// ---------------------------------------------AI 데이터 테이블--------------------------------------------- // 
void AHACKED_AI::InitAIStatByType(const EHackedAIType& type)
{
	aiStat = AI_Stat->GetAIStat(type);
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
}

float AHACKED_AI::GetMaxHp()
{
	return aiStat.MaxHp;
}

float AHACKED_AI::GetAIMoveSpeed()
{
	return aiStat.MoveSpeed;
}

float AHACKED_AI::GetAIPatrolRange()
{
	return aiStat.PatrolRange;
}

float AHACKED_AI::GetAttackRange()
{
	return aiStat.SR_CanAttackRange;
}

float AHACKED_AI::GetDetectingRange()
{
	return aiStat.PatrolDetectRange;
}

float AHACKED_AI::GetAIAttackDamage()
{
	return aiStat.SR_AttackDamage;
}
// -------------------------------------------------------------------------------------------------------- //


TTuple<AActor*, float> AHACKED_AI::GetNearestPlayer()
{
	if (_players.Num() <= 0) {
		RefreshPlayers();
		if (_players.Num() <= 0) {
			LOG(Error, "Can't find player..");
			return TTuple<AActor*, float>(nullptr, 100000.0f);
		}
	}
	float minDist = 1000000.0f;
	AHACKEDCharacter* minPlayer = nullptr;
	for (int i = 0; i < _players.Num(); ++i) {
		if(_players[i]->bIsSelfHealing) continue;
		float dist = FVector::Dist(_players[i]->GetActorLocation(), GetActorLocation());
		if (dist < minDist) {
			minDist = dist;
			minPlayer = _players[i];
		}
	}
	if (minPlayer) {
		NearestPlayerPos = minPlayer->GetActorLocation();
		return TTuple<AActor*, float>(minPlayer, minDist);
	}
	else {
		// 둘다없으면 제일 가까운 캐릭터에게 간다.
		for (int i = 0; i < _players.Num(); ++i) {
			float dist = FVector::Dist(_players[i]->GetActorLocation(), GetActorLocation());
			if (dist < minDist) {
				minDist = dist;
				minPlayer = _players[i];
			}
		}
		NearestPlayerPos = minPlayer->GetActorLocation();
		return TTuple<AActor*, float>(minPlayer, minDist);
	}
}

void AHACKED_AI::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HpComponent->OnDie.AddUObject(this, &AHACKED_AI::RPCDie);
}

void AHACKED_AI::OnSpawn()
{
	_bDie = false;
	currentStack = 0.0f;
	HpComponent->SetToMaxHP();
	GetCharacterMovement()->MaxWalkSpeed = GetAIMoveSpeed();
	NetTransformCP->SetSyncEnable(true);
	//LOG_SCREEN("AI Move Speed : %f", aiStat.aiMoveSpeed);
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("HACKED_AI"));
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	if(_meshOriginTransform.Equals(FTransform::Identity)) _meshOriginTransform = GetMesh()->GetRelativeTransform();
	GetMesh()->SetRelativeTransform(_meshOriginTransform,
		false, nullptr, ETeleportType::ResetPhysics);
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
}

void AHACKED_AI::OnSlotChange(bool isMaster)
{
	// 마스터를 제외한 클라이언트는 AI를 직접 돌리지 않도록 한다.
	if (isMaster) 
	{
		if (*_bOnAI) 
		{
			AIAllStopTool::AIRestart(this, true);
		}
		else
		{
			AIAllStopTool::AIDisable(this, true);
		}
	}
	else 
	{
		AIAllStopTool::AIDisable(this, true);
	}
}

void AHACKED_AI::RefreshPlayers()
{
	// Get all players
	_players.Empty();
	if (HACKEDInGameManager) {
		auto players = HACKEDInGameManager->GetPlayers();
		for (auto i : players)
			_players.Add(i);
	}
}

// Called when the game starts or when spawned
void AHACKED_AI::BeginPlay()
{
	Super::BeginPlay();

	auto damageOutput = Cast<UWGDamageOutput>(HackedAIWidget->GetUserWidgetObject());
	if (damageOutput) damageOutput->SetOwner(this);

	BindHACKEDInGameManager(HACKEDInGameManager);

	_bOnAI.SetReliable(true);
	_bOnAI.Init(&NetBaseCP, TEXT("_bOnAI"));
	BindRPCFunction(NetBaseCP, AHACKED_AI, RPCDie);
	BindRPCFunction(NetBaseCP, AHACKED_AI, RPCDestroyAI);
	BindRPCFunction(NetBaseCP, AHACKED_AI, RPCTakeDamageToOtherClient);
	BindRPCFunction(NetBaseCP, AHACKED_AI, RPCRigidityAI);
	BindRPCFunction(NetBaseCP, AHACKED_AI, RPCBeHitAnimation);
	BindRPCFunction(NetBaseCP, AHACKED_AI, RPCAddAgro);

	_spanwer = HACKEDInGameManager->GetSpawner();
	// 스포너로 만든 액터가 아니라면 오브젝트풀에 등록한다.
	if (_spanwer && _spanwer->CheckAndAddAlreadyExistObject(this)) {
		OnSpawn();
	}
	else {
		// 스포너로 생성된 액터는 꺼줍니다.
		_spanwer->ActiveObject(this, false);
	}

	_meshOriginTransform = GetMesh()->GetRelativeTransform();

	OnSlotChange(NetBaseCP->IsMaster());
}

float AHACKED_AI::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	// 클라이언트 본인 캐릭터의 공격만 처리한다.
	if (EventInstigator != UGameplayStatics::GetPlayerController(GetWorld(), 0)) return HpComponent->GetHP();
	if (IsDie()) return 0.0f;
		
	// 실제 TakeDamage 로직을 거침.
	float currentHp = HpComponent->GetHP();
	float retval = _TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	char damageOutput[30];
	_snprintf_s(damageOutput, sizeof(damageOutput), "DamageOutPut %f", currentHp - retval);
	FOutputDeviceNull ar;
	HackedAIWidget->GetUserWidgetObject()->CallFunctionByNameWithArguments(ANSI_TO_TCHAR(damageOutput), ar, NULL, true);

	// 깎인 값만큼 통보한다.
	RPCTakeDamageToOtherClient(NetBaseCP->GetCurrentSlot(), currentHp - retval);
	return retval;
}

float AHACKED_AI::_TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	AHACKED_ESPER* Esper = Cast<AHACKED_ESPER>(DamageCauser);
	AHACKED_CRUSHER* Crusher = Cast<AHACKED_CRUSHER>(DamageCauser);

	bool isAim = false;

	if (DamageEvent.IsOfType(FPlayerDamageEvent::ClassID)) {
		FPlayerDamageEvent* const playerDamageEvent = (FPlayerDamageEvent*)& DamageEvent;

		if (bCanRigidity && playerDamageEvent->bCanRigidity) {
			RPCRigidityAI();
		}
		else if (playerDamageEvent->bCanHitAnimation) {
			RPCBeHitAnimation();
		}


		if (Esper)
		{
			if (playerDamageEvent->bWithAim) {
				isAim = true;
			}

			if (Esper->bIsAdrenalSurge)
			{
				LOG_SCREEN("Esper AdrenalSurge On");
				DamageAmount *= 1.5f;
			}

			if (playerDamageEvent->bCanEsperStack)
			{
				currentStack += 0.25f;
				if (currentStack == 1.0f || currentStack == 2.0f || currentStack == 3.0f)
				{
					RPCBeHitAnimation();
				}
				//LOG_SCREEN("AI Current Stack : %f", currentStack);
				if (currentStack >= 4.0f)
				{
					RPCBeHitAnimation();
					DamageAmount += criticalDamage;
					currentStack = 3.0f;
				}
			}
			else if (playerDamageEvent->bCanExplosionEsperStack)
			{
				if (currentStack < 1.0f)
				{
					currentStack = 0.0f;
				}
				if (currentStack >= 1.0f && currentStack < 2.0f)
				{
					DamageAmount += firstStackDamage;
					currentStack = 0.0f;
				}
				else if (currentStack >= 2.0f && currentStack < 3.0f)
				{
					DamageAmount += secondStackDamage;
					currentStack = 0.0f;
				}
				else if (currentStack >= 3.0f && currentStack < 4.0f)
				{
					DamageAmount += thirdStackDamage;
					currentStack = 0.0f;
				}
			}
		}
		else if (Crusher)
		{
			if (Crusher->bIsAdrenalSurge)
			{
				DamageAmount *= 2.0f;
			}
		}
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageAmount = FMath::FRandRange(DamageAmount * 0.9f, DamageAmount * 1.1f);
	bTakenDamage = true;
	damageAmountOutput = (int)DamageAmount;

	if (isAim && Esper) {
		if(DamageAmount >= CRITICAL_DAMAGE_LIMIT) Esper->OnHitAim(true);
		else Esper->OnHitAim(false);
	}

	if (Esper)
	{
		RPCAddAgro(false, DamageAmount);

		if(!(Esper->bIsPsychicOverDrive))
		Esper->currentUltimateGauge += DamageAmount;
	}

	else if (Crusher)
	{
		RPCAddAgro(true, DamageAmount);

		if (!(Crusher->bIsAdrenalSurge) && !(Crusher->bCanTutoUltimate))
			Crusher->currentUltimateGauge += DamageAmount;
	}
	if (bIsAIImmortal) return HpComponent->GetHP();

	float hp = HpComponent->TakeDamage(DamageAmount);
	return HpComponent->GetHP();
}

void AHACKED_AI::RPCTakeDamageToOtherClient(int slot, float damage)
{
	RPC(NetBaseCP, AHACKED_AI, RPCTakeDamageToOtherClient, ENetRPCType::MULTICAST, true, slot, damage);

	// 이 함수에선 타인의 공격만 처리한다.
	// 이미 죽은 몬스터는 처리하지 않는다.
	if (NetBaseCP->GetCurrentSlot() == slot || IsDie()) return;
	
	// 직접 체력을 깎는다.
	HpComponent->TakeDamage(damage);
}

void AHACKED_AI::RPCAddAgro(bool isCrusher, int amount)
{
	RPC(NetBaseCP, AHACKED_AI, RPCAddAgro, ENetRPCType::MASTER, true, isCrusher, amount);

	if (isCrusher)
	{
		CrusherAgroGauge += amount;
	}
	else 
	{
		EsperAgroGauge += amount;
	}
}

bool AHACKED_AI::IsDie() {
	return _bDie;
}

void AHACKED_AI::DieSpawnEmitter()
{
	if (!_bDie)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PS_DieExplosion, GetActorLocation(), GetActorRotation(), true);
	}
}

void AHACKED_AI::RPCDie() 
{
	DieSpawnEmitter();
	RPC(NetBaseCP, AHACKED_AI, RPCDie, ENetRPCType::MULTICAST, true);
	DieProcess();
}

void AHACKED_AI::DieProcess()
{
	if (IsDie()) return;
	_bDie = true;

	//if (_bIsBossDie)
	//{
	//	Destroy();
	//}
	//else
	//{
		if (NetBaseCP->IsMaster()) GetWorldTimerManager().SetTimer(DestroyTimeHandle, this, &AHACKED_AI::RPCDestroyAI, 5.0f, false);
		AIAllStopTool::AIDisable(this);
		NetTransformCP->SetSyncEnable(false);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
		TArray<FName> bones = GetMesh()->GetAllSocketNames();

		if (this->IsA(AMilitaryBossAI::StaticClass()))
		{

		}
		else
		{
			for (auto i : bones)
			{
				if (i == TEXT("neck_01")) continue;
				if (i == TEXT("Bip001-Neck")) continue;
				if (i == TEXT("Bip001-L-Forearm")) continue;
				if (i == TEXT("Bip001-R-Forearm")) continue;
				if (i == TEXT("Bip001-Pelvis")) continue;
				if (i == TEXT("Bip001-Thigh")) continue;
				if (i == TEXT("Bip001-Spine")) continue;
				if (i == TEXT("Bip001-Head")) continue;
				GetMesh()->BreakConstraint(FVector::DownVector, GetActorLocation(), i);
			}
		}
	//}
	
}

void AHACKED_AI::RPCDestroyAI()
{
	RPC(NetBaseCP, AHACKED_AI, RPCDestroyAI, ENetRPCType::MULTICAST, true);
	if (_spanwer) _spanwer->DestroyObject(this);
	else Destroy();
}

void AHACKED_AI::SetDie(bool isDie) {
	_bDie = isDie;
}