// Fill out your copyright notice in the Description page of Project Settings.


#include "Phase1.h"
#include "CheckPoint.h"
#include "HackingPanel.h"
#include "../Spawner/WaveSystem.h"
#include "../AI/HACKED_AI.h"
#include "EmptyZoneFinder.h"
#include "CoreBattery.h"
#include "Components/WidgetComponent.h"
#include "InGame/HACKEDTool.h"
#include "Core/HACKEDGameInstance.h"
#include "Phase2.h"
#include "Interactor.h"
#include "PathIndicator.h"

APhase1::APhase1()
{
 	PrimaryActorTick.bCanEverTick = true;
	
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SOUND COMP"));
	subTitleVoiceComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SUBTITLE SOUND COMP"));
	hackingProgressWG = CreateDefaultSubobject<UWidgetComponent>(TEXT("HACKING PROGRESS"));

	static ConstructorHelpers::FObjectFinder<USoundBase>SB_FIGHT(TEXT("SoundWave'/Game/Sound/Phase/BattleBGM_Loop.BattleBGM_Loop'"));
	if (SB_FIGHT.Succeeded())
	{
		fightBGM = SB_FIGHT.Object;
		audioComp->SetSound(fightBGM);
	}

	hackingProgressWG->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> WG_hackingProgress(TEXT("/Game/Resources/Object/HackingPanel/HackingProgressBar.HackingProgressBar_C"));
	if (WG_hackingProgress.Succeeded())
	{
		hackingProgressWG->SetWidgetClass(WG_hackingProgress.Class);
		hackingProgressWG->SetDrawSize(FVector2D(300.0f, 30.0f));
	}
}

void APhase1::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(HACKEDInGameManager);
	audioComp->Stop();
	StartPhase1();
	LOG(Warning, "PHASE 1 START");
	hackingProgressWG->SetWorldLocation(hackingPanel->GetActorLocation()+hackingPanel->GetActorUpVector()*200.0f);
	hackingProgressWG->SetVisibility(false);
	
}

// Called every frame
void APhase1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsPhaseRunning)
	{
		RunningPhase1();
	}
}

void APhase1::StartPhase1()
{
	StartSector1();

	LOG(Warning, "SECTOR 1 START");
	TArray<AActor*>enemies = aiCheckZone->GetExistObject();

	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* enemy = Cast<AHACKED_AI>(iter);
			AIAllStopTool::AIDisable(enemy);
			enemy->bPatrolCheck = false;
			LOG(Warning, "%s STOPPED", *enemy->GetName());
		}
	}
	sector1PathIndicator->Start();
	
}

void APhase1::RunningPhase1()
{
	switch (currentSectorNumber)
	{
	case 1:
		RunningSector1();
		break;
	case 2:
		RunningSector2();
		break;
	case 3:
		RunningSector3();
		break;
	default:
		LOG(Warning, "PHASE NUMBER ERROR");
		break;
	}
}

void APhase1::EndPhase1()
{
	audioComp->FadeOut(5.0f, 0.0f);
	bIsPhaseComplete = true;
}

void APhase1::StartSector1()
{
	currentSectorNumber = 1;
	bIsPhaseRunning = true;
}

void APhase1::RunningSector1()
{
	if (ShelterOutCP->IsAnyEntered() && !shelterOutDoOnce)
	{
		shelterOutDoOnce = true;
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST3.1");
		subTitleVoiceComp->SetSound(st3);
		subTitleVoiceComp->Play();
		GetWorldTimerManager().SetTimer(voiceTiemr, this, &APhase1::Voice_FindCoreBattery, voiceTimerDelayInterval, false);
	}
	if (currentSectorNumber == 1 && Sector1CP->IsAnyEntered()&& !doOnce)
	{
		LOG_SCREEN("RUNNING SECTOR 1");
		waveSystem->StartWave(1);
		audioComp->FadeIn(5.0f);
		doOnce = true;
		//START ENEMY
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
		hackingPanel->SetActiveState(true);
		EndSector1();
	}
}

void APhase1::EndSector1()
{
	StartSector2();
	//LOG_SCREEN("CBAR");
	DUMMY->GetBase()->SetRenderCustomDepth(true);
	DUMMY->GetTop()->SetRenderCustomDepth(true);
}

void APhase1::StartSector2()
{
	currentSectorNumber = 2;
	doOnce = false;
}

void APhase1::RunningSector2()
{
	if (hackingPanel->GetRunningState() && !doOnce)
	{
		interactor->SetAsInteracted();
		LOG_SCREEN("RUNNING SECTOR 2");
		currentSectorNumber = 2;
		//Start Sector2 HackingPanel Timer : DEFAULT 30 SEC
		GetWorldTimerManager().SetTimer(Sector2Timer, this, &APhase1::EndSector2, hackingTime, false);
		hackingProgressWG->SetVisibility(true);
		//Start Sector3 Wave Timer : DEFAULT 20 SEC
		GetWorldTimerManager().SetTimer(sector3Timer, this, &APhase1::StartSector3, sector3SpawnTime, false);
		waveSystem->StartWave(2);
		doOnce = true;
		sector1PathIndicator->Stop();
		HACKEDInGameManager->GetQuestManager()->CompleteSubQuest(TEXT("SQ2"));
	}
	
}

void APhase1::EndSector2()
{
	LOG_SCREEN("END SECTOR 2");
	hackingPanel->EndHackingState();
	hackingPanel->SetHackingState(EHackingState::HACKING_END);
	DUMMY->OPEN();
	DUMMY->GetBase()->SetRenderCustomDepth(false);
	DUMMY->GetTop()->SetRenderCustomDepth(false);
	for (ADynamicPlayerBlocker* iter : blockers)
	{
		iter->OPEN();
	}
	hackingProgressWG->SetVisibility(false);
	
	Voice_AchieveEnergyCore();
	phase1sector2PathIndicator->Start();
}

void APhase1::StartSector3()
{
	currentSectorNumber = 3;
	doOnce = false;
}

void APhase1::RunningSector3()
{
	if (!doOnce)
	{
		LOG_SCREEN("RUNNING SECTOR 3");
		waveSystem->StartWave(3);
		doOnce = true;
	}
	
	//EXIST AI CHECK
	for (AActor* iter : aiCheckZone->GetExistObject())
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			return;
		}
		else
		{
			EndSector3();
		}
	}
}

void APhase1::EndSector3()
{
	//END PHASE
	EndPhase1();
}

void APhase1::Voice_FindCoreBattery()
{
	HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST3.2");
	subTitleVoiceComp->SetSound(st4);
	subTitleVoiceComp->Play();
}

void APhase1::Voice_AchieveEnergyCore()
{
	HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST3.3");
	subTitleVoiceComp->SetSound(energyCoreAcheive);
	subTitleVoiceComp->Play();
}

float APhase1::GetHackingPercent() const
{
	if (GetWorldTimerManager().IsTimerActive(Sector2Timer))
		return GetWorldTimerManager().GetTimerElapsed(Sector2Timer) / hackingTime;
	else
		return 0.0f;
}

