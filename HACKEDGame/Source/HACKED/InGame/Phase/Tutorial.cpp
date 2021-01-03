// Fill out your copyright notice in the Description page of Project Settings.


#include "Tutorial.h"
#include "Core/HACKEDGameInstance.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/MilitarySoldierAI/MilitarySoldierAI.h"
#include "InGame/AI/MilitaryNamedAI/MilitaryNamedAI.h"
#include "InGame/AI/MilitaryBossAI/HomingMissile.h"
#include "InGame/Phase/EmptyZoneFinder.h"


// Sets default values
ATutorial::ATutorial()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TutorialSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("TUTORIAL SOUND COMP"));
	IntroSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("INTRO SOUND COMP"));

	static ConstructorHelpers::FClassFinder<AMilitarySoldierAI> MilitarySoldier_C(TEXT("/Game/Blueprint/InGame/AI/BP_MilitarySoliderAI.BP_MilitarySoliderAI_C"));
	if (MilitarySoldier_C.Succeeded())
	{
		MilitarySoldierBP = MilitarySoldier_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AMilitaryNamedAI> MilitaryNamed_C(TEXT("/Game/Blueprint/InGame/AI/BP_MilitaryNamedAI.BP_MilitaryNamedAI_C"));
	if (MilitaryNamed_C.Succeeded())
	{
		MilitaryNamedBP = MilitaryNamed_C.Class;
	}

	static ConstructorHelpers::FClassFinder<AHomingMissile> HomingMissile_C(TEXT("/Game/Blueprint/InGame/AI/BP_HomingMissile.BP_HomingMissile_C"));
	if (HomingMissile_C.Succeeded())
	{
		HomingMissileBP = HomingMissile_C.Class;
	}

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
}


bool ATutorial::IsEsperPlay()
{
	return GetWorld()->GetFirstPlayerController()->GetPawn()->IsA(AHACKED_ESPER::StaticClass());
}

bool ATutorial::IsCrusherPlay()
{
	return GetWorld()->GetFirstPlayerController()->GetPawn()->IsA(AHACKED_CRUSHER::StaticClass());
}
void ATutorial::BeginPlay()
{
	Super::BeginPlay();

	BindHACKEDInGameManager(HACKEDInGameManager);
	BindRPCFunction(NetBaseCP, ATutorial, RPCTutorialSkip);
	BindRPCFunction(NetBaseCP, ATutorial, EsperUltimateSkillQuestClear);
	BindRPCFunction(NetBaseCP, ATutorial, CrusherUltimateQuestClear);
	BindRPCFunction(NetBaseCP, ATutorial, RPCCollisionDisable);


	Esper = HACKEDInGameManager->GetEsper();
	LOG(Warning, "Checking Esper : %s", *Esper->GetName());
	Crusher = HACKEDInGameManager->GetCrusher();
	LOG(Warning, "Checking Crusher : %s", *Crusher->GetName());

	if (bTutorialSkip) {
		RPCTutorialSkip();
	}
	else
	{
		Esper->SetEnableInput(false, false, false);
		Crusher->SetEnableInput(false, false, false);
		Esper->SkillDisable(false, false, false, false, false, false, false);
		Crusher->SkillDisable(false, false, false, false, false, false, false);
		Esper->bIsNowOnTutorial = true;
		Crusher->bIsNowOnTutorial = true;
	}
	RPCCollisionDisable();
}


void ATutorial::RPCTutorialSkip()
{
	RPC(NetBaseCP, ATutorial, RPCTutorialSkip, ENetRPCType::MULTICAST, true);
	TutorialSkip();
}

void ATutorial::TutorialSkip()
{
	Esper->bIsNowOnTutorial = false;
	Crusher->bIsNowOnTutorial = false;
	Esper->bIsImmortal = false;
	Crusher->bIsImmortal = false;
	Esper->SkillDisable();
	Crusher->SkillDisable();
	Esper->SetEnableInput(true, true, true);
	Crusher->SetEnableInput(true, true, true);
	Esper->bIsPsychicOverDrive = false;
	Crusher->bCanTutoUltimate = false;
	Esper->SetActorLocation(ShelterPos[0]->GetActorLocation());
	Crusher->SetActorLocation(ShelterPos[1]->GetActorLocation());
	Esper->currentUltimateGauge = 0.0f;
	Crusher->currentUltimateGauge = 0.0f;
	HACKEDInGameManager->GetSubtitleMaker()->StopSubtitle();
}

void ATutorial::ShelterSequence()
{
	HACKEDInGameManager->GetFadeInOutMaker()->FadeInOut(5.0f,0.0f);
	TutorialSoundComp->SetSound(ShelterStartSound);
	TutorialSoundComp->Play();

	GetWorldTimerManager().SetTimer(ShelterSequenceTimer, this, &ATutorial::RPCTutorialSkip, 5.0f,false);
	
}

void ATutorial::TutorialStart()
{
	TutorialSoundComp->SetSound(TutorialStartSound);
	TutorialSoundComp->Play();

	GetWorldTimerManager().SetTimer(TutorialSequenceTimer, this, &ATutorial::TutorialSequence, 5.0f, false);
}

void ATutorial::TutorialSequence()
{

	if (IsEsperPlay())
	{
		TutorialSoundComp->SetSound(EDQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ1"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("EQS1");
	}

	if (IsCrusherPlay())
	{
		TutorialSoundComp->SetSound(CDQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ1"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("CQS1");
	}

	Esper->bIsNowOnTutorial = true;
	Crusher->bIsNowOnTutorial = true;
	Esper->bIsImmortal = true;
	Crusher->bIsImmortal = true;
	Esper->bIsPsychicOverDrive = true;
	Crusher->bCanTutoUltimate = true;

	EsperDashQuest();
	CrusherDashQuest();
}

void ATutorial::RPCCollisionDisable()
{
	RPC(NetBaseCP, ATutorial, RPCCollisionDisable, ENetRPCType::MULTICAST, true);

	CollisionDisable();
}

void ATutorial::CollisionDisable()
{
	for (int i = 0; i < 2; i++)
	{
		ShelterPos[i]->SetActorEnableCollision(false);
		CooperationZonePos[i]->SetActorEnableCollision(false);
	}
	for (int i = 0; i < 3; i++)
	{
		EsperTutorialSpawner[i]->SetActorEnableCollision(false);
		CrusherTutorialSpawner[i]->SetActorEnableCollision(false);
		CooperationTutorialSpawner[i]->SetActorEnableCollision(false);
	}
}

void ATutorial::TutorialSpawner(ETutorialAIType aIType, TArray<AActor*> spawnPos, bool onlyOne)
{
	if (aIType == ETutorialAIType::MAX) {
		LOG(Error, "Wrong Type !!");
		return;
	}

	if (aIType == ETutorialAIType::MS_AI)
	{
		AMilitarySoldierAI* TutoSoldierAI;
		if (onlyOne) {
			TutoSoldierAI = GetWorld()->SpawnActor<AMilitarySoldierAI>(MilitarySoldierBP, spawnPos[0]->GetActorLocation(), GetActorRotation());
			if (!TutoSoldierAI) {
				LOG(Warning, "솔져 AI를 생성 할 수가 없습니다.");
				return;
			}
		}
		else {
			for (int i = 0; i < spawnPos.Num(); i++)
			{
				TutoSoldierAI = GetWorld()->SpawnActor<AMilitarySoldierAI>(MilitarySoldierBP, spawnPos[i]->GetActorLocation(), GetActorRotation());
				if (!TutoSoldierAI) {
					LOG(Warning, "솔져 AI를 생성 할 수가 없습니다.");
					return;
				}
			}
		}
	}
	else if(aIType == ETutorialAIType::MN_AI)
	{
		AMilitaryNamedAI* TutoNamedAI;
		if (onlyOne) {
			TutoNamedAI = GetWorld()->SpawnActor<AMilitaryNamedAI>(MilitaryNamedBP, spawnPos[0]->GetActorLocation(), GetActorRotation());
			if (!TutoNamedAI) {
				LOG(Warning, "네임드 AI를 생성 할 수가 없습니다.");
				return;
			}
		}
		else {
			for (int i = 0; i < spawnPos.Num(); i++)
			{
				TutoNamedAI = GetWorld()->SpawnActor<AMilitaryNamedAI>(MilitaryNamedBP, spawnPos[i]->GetActorLocation(), GetActorRotation());
				if (!TutoNamedAI) {
					LOG(Warning, "네임드 AI를 생성 할 수가 없습니다.");
					return;
				}
			}
		}
	}
}

void ATutorial::TutoMissileSpawner()
{
	if (!bIsCrusherFirstStart) return;
	if (bIsCrusherFirstStart) {
		if (HACKEDInGameManager->GetCrusher()->BarrierHitCount >= 1) 
		{
			QuestClearSubtileShow(false);
			GetWorldTimerManager().ClearTimer(MissileSpawnTimer);
			CrusherFirstQuestClear();
			return;
		}
	}
	
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.Instigator = Instigator;
	AHomingMissile* LaunchTutoMissile = nullptr;
	LaunchTutoMissile = GetWorld()->SpawnActor<AHomingMissile>(HomingMissileBP, MissileSpawner->GetActorLocation(), MissileSpawner->GetActorRotation(), spawnParams);
	if (!LaunchTutoMissile) return;
	LaunchTutoMissile->Target = Crusher;

	if (LaunchTutoMissile != nullptr)
	{
		LOG_SCREEN("Can't Launch Missile");
		FRotator missileRot = LaunchTutoMissile->MissileMesh->GetComponentRotation();
		missileRot.Roll = 0.0f;
		missileRot.Pitch = -90.0f;
		missileRot.Yaw = 0.0f;
		LaunchTutoMissile->MissileMesh->SetRelativeRotation(missileRot);
	}
}

void ATutorial::EsperAIZoneCheck()
{
	TArray<AActor*> tutorialAI = EsperTutorialZoneCheckAI->GetExistObject();
	for (AActor* iter : tutorialAI)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* tutoAI = Cast<AHACKED_AI>(iter);

			if (tutoAI->IsValidLowLevel())
			{
				tutoAI->GetCharacterMovement()->MaxWalkSpeed = tutoAI->GetAIMoveSpeed();	
				EsperZoneEnemyCheck = true;	
				++EsperZoneEnemyCount;
			}	
		}
	}
	if (EsperZoneEnemyCount == 0)
	{
		EsperZoneEnemyCheck = false;
	}
	EsperZoneEnemyCount = 0;
}

void ATutorial::CrusherAIZoneCheck()
{
	TArray<AActor*> tutorialAI = CrusherTutorialZoneCheckAI->GetExistObject();
	for (AActor* iter : tutorialAI)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* tutoAI = Cast<AHACKED_AI>(iter);
			AMilitarySoldierAI* aiForBarrierCheck = Cast<AMilitarySoldierAI>(tutoAI);

			if (tutoAI->IsValidLowLevel())
			{
				tutoAI->GetCharacterMovement()->MaxWalkSpeed = tutoAI->GetAIMoveSpeed();
				CrusherZoneEnemyCheck = true;
				++CrusherZoneEnemyCount;
			}
		}
	}
	if (CrusherZoneEnemyCount == 0)
	{
		CrusherZoneEnemyCheck = false;
	}
	CrusherZoneEnemyCount = 0;
}

void ATutorial::CooperationZoneAICheck()
{
	TArray<AActor*> tutorialAI = CooperationZoneCheckAI->GetExistObject();
	for (AActor* iter : tutorialAI)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* tutoAI = Cast<AHACKED_AI>(iter);
			if (tutoAI->IsValidLowLevel())
			{
				tutoAI->GetCharacterMovement()->MaxWalkSpeed = tutoAI->GetAIMoveSpeed();
				CooperationZoneEnemyCheck = true;
				++CooperationZoneEnemyCount;
			}
		}
	}
	if (CooperationZoneEnemyCount == 0)
	{
		CooperationZoneEnemyCheck = false;
	}
	CooperationZoneEnemyCount = 0;
}

void ATutorial::QuestClearSubtileShow(bool isEsper)
{
	if (isEsper) {
		if (IsEsperPlay()) {
			TutorialSoundComp->Stop();
			TutorialSoundComp->SetSound(QuestClearSound);
			TutorialSoundComp->Play();
			HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("TQCS1");
		}
	}
	else
	{
		if (IsCrusherPlay()) {
			TutorialSoundComp->Stop();
			TutorialSoundComp->SetSound(QuestClearSound);
			TutorialSoundComp->Play();
			HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("TQCS2");
		}
	}
	
}

void ATutorial::EsperDashQuest()
{
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), EPQ_Sound, EsperTutorialZoneCheckAI->GetActorLocation(), 1.0f, 1.0f, 0.0f, Tutorial_SoundAttenuation);
	LOG(Warning, "Esper Dash Quest Start");
	Esper->SetEnableInput(true, true, true);
	Esper->SkillDisable(false,false, false, false, false, true, false);
	bIsEsperDashStart = true;
}

void ATutorial::EsperDashQuestClear()
{
	LOG(Warning, "Esper Dash Quest Clear");
	TutorialSpawner(ETutorialAIType::MS_AI, EsperTutorialSpawner, true);
	bIsEsperDashStart = false;
	_bEsperStartOnce = true;
	bIsEsperPrimaryStart = true;
}

void ATutorial::EsperPrimaryQuest()
{
	//TutorialSoundComp->SetSound(EPQ_Sound);
	//TutorialSoundComp->Play();
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), EPQ_Sound, EsperTutorialZoneCheckAI->GetActorLocation(), 1.0f,1.0f,0.0f, Tutorial_SoundAttenuation);
	LOG(Warning, "Esper Primary Attack Quest Start");
	if (IsEsperPlay()) {
		TutorialSoundComp->SetSound(EPQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ2"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("EQS2"));
	}
	Esper->SkillDisable(true, false, false, false, false, true, false);
}

void ATutorial::EsperPrimaryQuestClear()
{
	LOG(Warning, "Esper Primary Attack Quest Clear");
	bIsEsperPrimaryStart = false;
	_bEsperStartOnce = true;
	bIsEsperM2Start = true;
}

void ATutorial::EsperSkillM2Quest()
{
	LOG(Warning, "Esper M2 Skill Quest Start");
	if (IsEsperPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(EM2Q_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ3"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("EQS3"));
	}
	Esper->PsychicDropDamageChange(400.0f);
	Esper->SkillDisable(false, true, false, false, false, true, false);
	TutorialSpawner(ETutorialAIType::MS_AI, EsperTutorialSpawner, true);
}

void ATutorial::EsperSkillM2QuestClear()
{
	LOG(Warning, "Esper M2 Skill Quest Clear");
	Esper->PsychicDropDamageChange(-400.0f);
	bIsEsperM2Start = false;
	_bEsperStartOnce = true;
	bIsEsperFirstStart = true;
}

void ATutorial::EsperFirstSkillQuest()
{
	LOG(Warning, "Esper First Skill Quest Start");
	if (IsEsperPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(EFQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ4"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("EQS4"));
	}
	Esper->SkillDisable(false, false, true, false, false, true, false);
	Esper->PsychicForceDamageChange(250.0f);
	TutorialSpawner(ETutorialAIType::MS_AI, EsperTutorialSpawner, false);
}

void ATutorial::EsperFirstSkillQuestClear()
{
	LOG(Warning, "Esper First Skill Quest Clear");
	bIsEsperFirstStart = false;
	Esper->PsychicForceDamageChange(-250.0f);
	_bEsperStartOnce = true;
	bIsEsperSecondStart = true;
}

void ATutorial::EsperSecondSkillQuest()
{
	LOG(Warning, "Esper Second Skill Quest Start");
	if (IsEsperPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(ESQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ5"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("EQS5"));
	}
	Esper->PsychicDropDamageChange(30.0f);
	Esper->SkillDisable(false, false, false, true, false, true, false);
	TutorialSpawner(ETutorialAIType::MS_AI, EsperTutorialSpawner, false);
}

void ATutorial::EsperSecondSkillQuestClear()
{
	LOG(Warning, "Esper Second Skill Quest Cleaar");
	Esper->PsychicDropDamageChange(-30.0f);
	bIsEsperSecondStart = false;
	_bEsperStartOnce = true;
	bIsEsperUltimateStart = true;
}

void ATutorial::EsperUltimateSkillQuest()
{
	LOG(Warning, "Esper Ultimate Skill Quest Start");
	if (IsEsperPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(EUQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("ETQ6"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("EQS6"));
	}
	Esper->SkillDisable(true, true, true, true, true, true, true);
	Esper->bIsPsychicOverDrive = false;
	TutorialSpawner(ETutorialAIType::MS_AI, EsperTutorialSpawner, false);
	EsperSpawnDelay = 21.0f;
	GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperUltimateQuestSpawn, 7.0f, true);
}

void ATutorial::EsperUltimateQuestSpawn()
{
	EsperSpawnDelay -= 7.0f;
	TutorialSpawner(ETutorialAIType::MS_AI, EsperTutorialSpawner, false);
	if (EsperSpawnDelay <= 0.0f)
	{
		bIsEsperFinalSpawncheck = true;
		GetWorldTimerManager().ClearTimer(EsperSpawnDelayTimer);
	}
}

void ATutorial::EsperUltimateSkillQuestClear()
{
	RPC(NetBaseCP, ATutorial, EsperUltimateSkillQuestClear, ENetRPCType::MULTICAST, true);

	Esper->SetActorLocation(CooperationZonePos[0]->GetActorLocation());
	Esper->SetEnableInput(true, true, true);
	Esper->bIsNowOnCooperationTuto = true;
}

void ATutorial::CrusherDashQuest()
{
	LOG(Warning, "Crusher Dash Quest Start");
	Crusher->SetEnableInput(true, true, false);
	Crusher->SkillDisable(false, false, false, false, false, true, false);
	bIsCrusherDashStart = true;
}

void ATutorial::CrusherDashQuestClear()
{
	LOG(Warning, "Crusher Dash Quest Clear");
	TutorialSpawner(ETutorialAIType::MS_AI, CrusherTutorialSpawner, false);
	bIsCrusherDashStart = false;
	_bCrusherStartOnce = true;
	bIsCrusherPrimaryStart = true;
}

void ATutorial::CrusherPrimaryQuest()
{
	LOG(Warning, "Crusher Primary Quest Start");
	if (IsCrusherPlay()) {
		TutorialSoundComp->SetSound(CPQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ2"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("CQS2"));
	}
	Crusher->SetEnableInput(true, true, true);
	Crusher->SkillDisable(true, false, false, false, false, true, false);
}

void ATutorial::CrusherPrimaryQuestClear()
{
	LOG(Warning, "Crusher Primary Quest Clear");
	bIsCrusherPrimaryStart = false;
	_bCrusherStartOnce = true;
	bIsCrusherM2Start = true;
}

void ATutorial::CrusherM2Quest()
{
	LOG(Warning, "Crusher M2 Quest Start");
	if (IsCrusherPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(CM2Q_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ3"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("CQS3"));
	}
	Crusher->SetEnableInput(true, true, true);
	Crusher->SkillDisable(false, true, false, false, false, true, false);
	Crusher->RushDamageChange(500.0f);
	TutorialSpawner(ETutorialAIType::MS_AI, CrusherTutorialSpawner, true);
}

void ATutorial::CrusherM2QuestClear()
{
	LOG(Warning, "Crusher M2 Quest Clear");
	bIsCrusherM2Start = false;
	Crusher->RushDamageChange(-500.0f);
	_bCrusherStartOnce = true;
	bIsCrusherFirstStart = true;
}

void ATutorial::CrusherFirstQuest()
{
	LOG(Warning, "Crusher First Skill Start");
	if (IsCrusherPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(CFQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ4"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("CQS4"));
	}
	Crusher->SkillDisable(false, false, true, false, false, true, false);
	MissileSpawnerMesh->GetStaticMeshComponent()->SetRenderCustomDepth(true);
	MissileSpawnerBodyMesh->GetStaticMeshComponent()->SetRenderCustomDepth(true);
	GetWorldTimerManager().SetTimer(MissileSpawnTimer, this, &ATutorial::TutoMissileSpawner, MissileSpawnDelay * 0.5f, true);
}

void ATutorial::CrusherFirstQuestClear()
{
	LOG(Warning, "Crusher First Skill Clear");
	MissileSpawnerMesh->GetStaticMeshComponent()->SetRenderCustomDepth(false);
	MissileSpawnerBodyMesh->GetStaticMeshComponent()->SetRenderCustomDepth(false);
	bIsCrusherFirstStart = false;
	_bCrusherStartOnce = true;
	bIsCrusherSecondStart = true;
}

void ATutorial::CrusherSecondQuest()
{
	LOG(Warning, "Crusher Second Skill Start");
	if (IsCrusherPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(CSQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ5"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("CQS5"));
	}
	Crusher->PunishDamageChange(100.0f);
	Crusher->SkillDisable(false, false, false, true, false, true, false);
	TutorialSpawner(ETutorialAIType::MS_AI, CrusherTutorialSpawner, false);
}

void ATutorial::CrusherSecondQuestClear()
{
	Crusher->PunishDamageChange(-100.0f);
	bIsCrusherSecondStart = false;
	_bCrusherStartOnce = true;
	bIsCrusherUltimateStart = true;
}

void ATutorial::CrusherUltimateQuest()
{
	LOG(Warning, "Crusher Ultimate Skill Quest Start");
	if (IsCrusherPlay()) {
		TutorialSoundComp->Stop();
		TutorialSoundComp->SetSound(CUQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("CTQ6"));
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("CQS6"));

	}
	Crusher->SkillDisable(true, true, true, true, true, true, false);
	Crusher->bCanTutoUltimate = false; 
	TutorialSpawner(ETutorialAIType::MS_AI, CrusherTutorialSpawner, false);
	CrusherSpawnDelay = 14.0f;
	GetWorldTimerManager().SetTimer(CrusherSpawnDelayTimer, this, &ATutorial::CrusherUltimateQuestSpawn, 7.0f, true);
}

void ATutorial::CrusherUltimateQuestSpawn()
{
	CrusherSpawnDelay -= 7.0f;
	TutorialSpawner(ETutorialAIType::MS_AI, CrusherTutorialSpawner, false);
	if (CrusherSpawnDelay <= 0.0f)
	{
		bIsCrusherFinalSpawnCheck = true;
		GetWorldTimerManager().ClearTimer(CrusherSpawnDelayTimer);
	}

}

void ATutorial::CrusherUltimateQuestClear()
{
	RPC(NetBaseCP, ATutorial, CrusherUltimateQuestClear, ENetRPCType::MULTICAST, true);

	Crusher->SetActorLocation(CooperationZonePos[1]->GetActorLocation());
	Crusher->SetEnableInput(true, true, true);
	Crusher->bIsNowOnCooperationTuto = true;
}

void ATutorial::CooperationFirstQuest()
{
	if (IsCrusherPlay()) {
		TutorialSoundComp->SetSound(CZFQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("TQS1"));
	}
	HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("TTQ1"));
	bIsCooperationFirstStart = true;
	Crusher->SkillDisable(true, true, true, true, true, true, true);
	Esper->SkillDisable(true, true, true, true, true, true, true);
}

void ATutorial::CooperationFirstQuestClear()
{
	bIsCooperationFirstStart = false;
	_bCooperationStartOnce = true;
	bIsCooperationSecondStart = true;
}

void ATutorial::CooperationSecondQuest()
{
	Esper->bIsImmortal = false;
	Crusher->bIsImmortal = false;
	TutorialSpawner(ETutorialAIType::MN_AI, CooperationTutorialSpawner, false);
}

void ATutorial::CooperationSecondQuestClear()
{
	_bCooperationStartOnce = true;
	bIsCooperationSecondStart = false;
	bIsCooperationThirdStart = true;
}

void ATutorial::CooperationThirdQuest()
{
	if (IsEsperPlay()) {
		TutorialSoundComp->SetSound(CZSQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("TQS2"));
	}
	HACKEDInGameManager->GetQuestManager()->StartQuest(TEXT("TTQ2"));
	Esper->SkillDisable(false, false, false, false, true, false, false);
	Esper->PsychicShieldTimeChange(100.0f);
	Crusher->SkillDisable(false, false, false, false, false, false, false);
}

void ATutorial::CooperationThirdQuestClear()
{
	bIsCooperationThirdStart = false;
	_bCooperationStartOnce = true;
	bIsCooperationFourthStart = true;
}

void ATutorial::CooperationFinalQuest()
{
	if (IsCrusherPlay()) {
		TutorialSoundComp->SetSound(CZTQ_Sound);
		TutorialSoundComp->Play();
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("TQS3"));
	}
	Crusher->SkillDisable(false, false, false, false, true, false, false);
}

void ATutorial::CooperationFinalQuestClear()
{
	Esper->PsychicShieldTimeChange(-100.0f);
	Esper->PsychicShieldDestroy();
	ShelterSequence();
}

// Called every frame
void ATutorial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//LOG(Warning, "bCanPrimary : %d ,bCanM2 : %d , bCanFirst : %d, bCanSecond : %d, bCanThird : %d", Esper->bCanPrimary, Esper->bIsM2CoolDown, Esper->bIsFirstCoolDown, Esper->bIsSecondCoolDown, Esper->bIsThirdCoolDown);
	if (Esper->bIsNowOnTutorial)
	{
		if (bIsEsperDashStart)
		{
			if (Esper->DashCount >= 3 && _bEsperClearOnce) // 에스퍼 대쉬 퀘스트 클리어 
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ1"));
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ2"));
				QuestClearSubtileShow(true);
				GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperDashQuestClear, SpawnDelayTime, false);
				_bEsperClearOnce = false;
			}
		}
		else if (bIsEsperPrimaryStart)
		{
			if (_bEsperStartOnce) {
				EsperPrimaryQuest();
				_bEsperStartOnce = false;
				_bEsperClearOnce = true;
			}
			EsperAIZoneCheck();
			if (!EsperZoneEnemyCheck && _bEsperClearOnce) // 에스퍼 평타 퀘스트 클리어 
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ3"));
				QuestClearSubtileShow(true);
				GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperPrimaryQuestClear, SpawnDelayTime, false);
				_bEsperClearOnce = false;
			}
		}
		else if (bIsEsperM2Start)
		{
			if (_bEsperStartOnce) {
				EsperSkillM2Quest();
				_bEsperStartOnce = false;
				_bEsperClearOnce = true;
			}
			EsperAIZoneCheck();
			if (!EsperZoneEnemyCheck && _bEsperClearOnce) // 에스퍼 마우스 우클릭 퀘스트 클리어 
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ4"));
				QuestClearSubtileShow(true);
				GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperSkillM2QuestClear, SpawnDelayTime, false);
				_bEsperClearOnce = false;
			}
		}
		else if (bIsEsperFirstStart)
		{
			if (_bEsperStartOnce) {
				EsperFirstSkillQuest();
				_bEsperStartOnce = false;
				_bEsperClearOnce = true;

			}
			EsperAIZoneCheck();
			if (!EsperZoneEnemyCheck && _bEsperClearOnce) // 에스퍼 1번 스킬 퀘스트 클리어 
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ5"));
				QuestClearSubtileShow(true);
				GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperFirstSkillQuestClear, SpawnDelayTime, false);
				_bEsperClearOnce = false;
			}
		}
		else if (bIsEsperSecondStart)
		{
			if (_bEsperStartOnce) {
				EsperSecondSkillQuest();
				_bEsperStartOnce = false;
				_bEsperClearOnce = true;
			}
			EsperAIZoneCheck();
			if (!EsperZoneEnemyCheck && _bEsperClearOnce)
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ6"));
				QuestClearSubtileShow(true);
				GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperSecondSkillQuestClear, SpawnDelayTime, false);
				_bEsperClearOnce = false;
			}
		}
		else if (bIsEsperUltimateStart)
		{
			if (_bEsperStartOnce) {
				EsperUltimateSkillQuest();
				_bEsperStartOnce = false;
				_bEsperClearOnce = true;
			}
			if (bIsEsperFinalSpawncheck) {
				EsperAIZoneCheck();
				if (bIsEsperSkillDisable && Esper->currentUltimateGauge >= Esper->maxUltimateGauge) {
					HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ7"));
					Esper->SkillDisable(false, false, false, false, false, true, true);
					bIsEsperSkillDisable = false;
				}
				if (!EsperZoneEnemyCheck && _bEsperClearOnce) {
					HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("ETSQ8"));
					Esper->SetEnableInput(false, false, false);
					if (IsEsperPlay())
					{
						TutorialSoundComp->SetSound(GoToCooperationSound);
						TutorialSoundComp->Play();
					}
					GetWorldTimerManager().SetTimer(EsperSpawnDelayTimer, this, &ATutorial::EsperUltimateSkillQuestClear, SpawnDelayTime * 2, false); // 협동존으로 이동하는 코드 (크러셔가 먼저 도달했을 경우 바로 협동 퀘스트 시작 그게 아닐경우 기다리라는 문구 띄우기) 
					_bEsperClearOnce = false;
				}
			}
		}
	}

	if (Crusher->bIsNowOnTutorial)
	{
		if (bIsCrusherDashStart)
		{
			if (Crusher->DashCount >= 3 && _bCrusherClearOnce)
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ1"));
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ2"));
				QuestClearSubtileShow(false);
				GetWorldTimerManager().SetTimer(CrusherSpawnDelayTimer, this, &ATutorial::CrusherDashQuestClear, SpawnDelayTime, false);
				_bCrusherClearOnce = false;
			}
		}
		else if (bIsCrusherPrimaryStart)
		{
			if (_bCrusherStartOnce) {
				CrusherPrimaryQuest();
				_bCrusherStartOnce = false;
				_bCrusherClearOnce = true;
			}
			CrusherAIZoneCheck();
			if (!CrusherZoneEnemyCheck && _bCrusherClearOnce)
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ3"));
				QuestClearSubtileShow(false);
				GetWorldTimerManager().SetTimer(CrusherSpawnDelayTimer, this, &ATutorial::CrusherPrimaryQuestClear, SpawnDelayTime, false);
				_bCrusherClearOnce = false;
			}
		}
		else if (bIsCrusherM2Start)
		{
			if (_bCrusherStartOnce)
			{
				CrusherM2Quest();
				_bCrusherStartOnce = false;
				_bCrusherClearOnce = true;
			}
			CrusherAIZoneCheck();
			if (!CrusherZoneEnemyCheck && _bCrusherClearOnce)
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ4"));
				QuestClearSubtileShow(false);
				GetWorldTimerManager().SetTimer(CrusherSpawnDelayTimer, this, &ATutorial::CrusherM2QuestClear, SpawnDelayTime, false);
				_bCrusherClearOnce = false;
			}
		}
		else if (bIsCrusherFirstStart)
		{
			if (_bCrusherStartOnce) {
				CrusherFirstQuest();
				_bCrusherStartOnce = false;
				_bCrusherClearOnce = true;
			}			
		}	
		else if (bIsCrusherSecondStart)
		{
			if (_bCrusherStartOnce) {
				CrusherSecondQuest();
				_bCrusherStartOnce = false;
				_bCrusherClearOnce = true;
			}
			CrusherAIZoneCheck();
			if (!CrusherZoneEnemyCheck && _bCrusherClearOnce)
			{
				HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ6"));
				QuestClearSubtileShow(false);
				GetWorldTimerManager().SetTimer(CrusherSpawnDelayTimer, this, &ATutorial::CrusherSecondQuestClear, SpawnDelayTime, false);
				_bCrusherClearOnce = false;
			}
		}
		else if (bIsCrusherUltimateStart)
		{
			if (_bCrusherStartOnce) {
				CrusherUltimateQuest(); 
				_bCrusherStartOnce = false;
				_bCrusherClearOnce = true;
			}
			if (Crusher->currentUltimateGauge >= Crusher->maxUltimateGauge) {
				if (_bCrusherGaugeCheckOnce)
				{
					HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ8"));
					_bCrusherGaugeCheckOnce = false;
				}

				if (bIsCrusherFinalSpawnCheck) {
					CrusherAIZoneCheck(); 
					if (!CrusherZoneEnemyCheck && _bCrusherClearOnce) {
						// 궁극기 게이지를 다채우는 퀘스트 완료 
						HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CTSQ7"));
						Crusher->SetEnableInput(false, false, false);
						if (IsCrusherPlay())
						{
							TutorialSoundComp->SetSound(GoToCooperationSound);
							TutorialSoundComp->Play();
						}
						GetWorldTimerManager().SetTimer(CrusherSpawnDelayTimer, this, &ATutorial::CrusherUltimateQuestClear, SpawnDelayTime * 2, false); // 협동존으로 이동하는 코드 (에스퍼가 먼저 도달했을 경우 바로 협동 퀘스트 시작 그게 아닐경우 기다리라는 문구 띄우기) 
						_bCrusherClearOnce = false;
					}
				}
				
			}
		}
	}
	if (Esper->bIsNowOnCooperationTuto && Crusher->bIsNowOnCooperationTuto && bIsCooperationStart)
	{
		LOG(Warning, "%d , %d", Esper->bIsNowOnCooperationTuto, Crusher->bIsNowOnCooperationTuto);

		GetWorldTimerManager().SetTimer(CooperationZoneDelayTimer, this, &ATutorial::CooperationFirstQuest, SpawnDelayTime, false);
		bIsCooperationStart = false;	
	}

	if (bIsCooperationFirstStart)
	{
		if (Crusher->bCooperationUltimateCheck && _bCooperationStartOnce)
		{
			LOG(Warning, "Crusher Ultimate Using Quest Clear");
			HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CZSQ1"));
			if (IsCrusherPlay()) {
				QuestClearSubtileShow(false);
			}
			GetWorldTimerManager().SetTimer(CooperationZoneDelayTimer, this, &ATutorial::CooperationFirstQuestClear, SpawnDelayTime, false);
			_bCooperationStartOnce = false;
		}
	}
	else if (bIsCooperationSecondStart)
	{
		if (_bCooperationStartOnce) {
			CooperationSecondQuest();
			_bCooperationStartOnce = false;
			_bCooperationClearOnce = true;
		}
		CooperationZoneAICheck();
		if (!CooperationZoneEnemyCheck && _bCooperationClearOnce) {
			HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CZSQ2"));
			GetWorldTimerManager().SetTimer(CooperationZoneDelayTimer, this, &ATutorial::CooperationSecondQuestClear, SpawnDelayTime, false);
			_bCooperationClearOnce = false;
		}
	}
	else if (bIsCooperationThirdStart)
	{
		if (_bCooperationStartOnce) {
			CooperationThirdQuest();
			_bCooperationStartOnce = false;
			_bCooperationClearOnce = true;
		}

		if ((Esper->GetShieldRate() > 0.1f && Crusher->GetShieldRate() > 0.1f) &&_bCooperationClearOnce) {
			LOG(Warning, "%f, %f", Esper->GetShieldRate(), Crusher->GetShieldRate());
			HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CZSQ3"));
			_bCooperationClearOnce = false;
			GetWorldTimerManager().SetTimer(CooperationZoneDelayTimer, this, &ATutorial::CooperationThirdQuestClear, SpawnDelayTime, false);
		}

	}
	else if (bIsCooperationFourthStart)
	{
		if (_bCooperationStartOnce) {
			CooperationFinalQuest();
			_bCooperationStartOnce = false;
			_bCooperationClearOnce = true;
		}

		if (Crusher->tutoVitalityUsedCheck && _bCooperationClearOnce)
		{
			HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("CZSQ4"));
			HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode(TEXT("TQS4"));
			TutorialSoundComp->SetSound(AllTutoClearSound);
			TutorialSoundComp->Play();
			_bCooperationClearOnce = false;
			GetWorldTimerManager().SetTimer(CooperationZoneDelayTimer, this, &ATutorial::CooperationFinalQuestClear, 7.0f, false);
		}
	}


}

