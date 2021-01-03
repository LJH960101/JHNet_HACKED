// Fill out your copyright notice in the Description page of Project Settings.


#include "Phase2.h"
#include "CheckPoint.h"
#include "HackingPanel.h"
#include "../Spawner/WaveSystem.h"
#include "../AI/HACKED_AI.h"
#include "EmptyZoneFinder.h"
#include "Phase1.h"
#include "Phase3.h"
#include "Tunnel.h"
#include "Core/HACKEDGameInstance.h"
#include "PathIndicator.h"

APhase2::APhase2()
{
	PrimaryActorTick.bCanEverTick = true;
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SOUND COMP"));
	subTitleVoiceComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SUBTITLE SOUND COMP"));
	static ConstructorHelpers::FObjectFinder<USoundBase>SB_FIGHT(TEXT("SoundWave'/Game/Sound/Phase/BattleBGM_Loop.BattleBGM_Loop'"));
	if (SB_FIGHT.Succeeded())
	{
		fightBGM = SB_FIGHT.Object;
		audioComp->SetSound(fightBGM);
	}
}

void APhase2::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(HACKEDInGameManager);
	audioComp->Stop();
	TArray<AActor*>enemies = aiCheckZone->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* enemy = Cast<AHACKED_AI>(iter);
			AIAllStopTool::AIDisable(enemy);
			enemy->bPatrolCheck = false;
		}
	}
	phase1sector2PathIndicator->Stop();
}

// Called every frame
void APhase2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (phase1EndCP->IsAnyEntered() && phase1->GetPhase1CompleteState()&& !doOncePhase1)
	{
		doOncePhase1 = true;
		StartPhase2();
	}

	if (bIsPhaseRunning)
	{
		RunningPhase2();
	}
}

void APhase2::StartPhase2()
{
	TArray<AActor*>enemies = aiCheckZone->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* enemy = Cast<AHACKED_AI>(iter);
			AIAllStopTool::AIRestart(enemy);
			enemy->bPatrolCheck = true;
		}
	}
	bIsPhaseRunning = true;
	FOutputDeviceNull ar;
	tunnel->OutlineOn();
	HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST4");
	subTitleVoiceComp->SetSound(st5);
	subTitleVoiceComp->Play();
}

void APhase2::StartQuest()
{
	HACKEDInGameManager->GetQuestManager()->StartQuest("MQ4");
}

void APhase2::RunningPhase2()
{
	if (phase2StartCP->IsAnyEntered() && !doOnce)
	{
		waveSystem->StartWave(4);
		audioComp->FadeIn(5.0f);
		doOnce = true;
		bIsTunnelEntered = true;
		FOutputDeviceNull ar;
		tunnel->OutlineOff();
		phase1sector2PathIndicator->Stop();
		//AFTER 5 SECONDS DO AI COUNT

		HACKEDInGameManager->GetQuestManager()->CompleteSubQuest("SQ4");
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST5");
		subTitleVoiceComp->SetSound(st6);
		subTitleVoiceComp->Play();
		GetWorldTimerManager().SetTimer(delayTimer, this, &APhase2::DelayEnemyCount, 5.0f, false);
		GetWorldTimerManager().SetTimer(questTimer, this, &APhase2::StartQuest, 2.0f, false);
	}

	//AI ALIVE CHECK
	TArray<AActor*>enemies = aiCheckZone->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			++enemyCount;
			break;
		}

	}

	//AI COUNT & PHASE ENDING CHECK
	if (enemyCount == 0 && bIsTunnelEntered && bIsDelayFinished&& !doOncePhase2)
	{
		EndPhase2();
	}
	enemyCount = 0;
}

void APhase2::EndPhase2()
{
	doOncePhase2 = true;

	HACKEDInGameManager->GetQuestManager()->CompleteSubQuest("SQ5");
	phase3->StartPhase3();
	audioComp->FadeOut(5.0f,0.0f);
	bIsPhaseRunning = false;
}

void APhase2::DelayEnemyCount()
{
	bIsDelayFinished = true;
}
