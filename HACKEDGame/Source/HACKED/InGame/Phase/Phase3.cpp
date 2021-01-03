// Fill out your copyright notice in the Description page of Project Settings.

#include "Phase3.h"
#include "CheckPoint.h"
#include "HackingPanel.h"
#include "../Spawner/WaveSystem.h"
#include "../AI/HACKED_AI.h"
#include "EmptyZoneFinder.h"
#include "Core/HACKEDGameInstance.h"
#include "PathIndicator.h"
#include "ExplosiveObject.h"
#include "PhaseBoss.h"
#include "InGame/Phase/DynamicPlayerBlocker.h"
#include "JHNET/InGame/Network/InGameNetworkProcessor.h"

APhase3::APhase3()
{
	PrimaryActorTick.bCanEverTick = true;
	subTitleVoiceComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SUBTITLE SOUND COMP"));

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
}

void APhase3::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(HACKEDInGameManager);

	BindRPCFunction(NetBaseCP, APhase3, BossProduction);
	BindRPCFunction(NetBaseCP, APhase3, BossProductionStart);
	BindRPCFunction(NetBaseCP, APhase3, RPCGoToBossCheat);

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
	phase2to3PathIndicator->Stop();
	blocker->OPEN();
}

// Called every frame
void APhase3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPhaseRunning)
	{
		RunningPhase3();
	}
}

void APhase3::RPCGoToBossCheat()
{
	RPC(NetBaseCP, APhase3, RPCGoToBossCheat, ENetRPCType::MULTICAST, true);
	StartPhase3();
	BossProductionStart();
}

void APhase3::StartPhase3()
{
	bIsPhaseRunning = true;
	phase2to3PathIndicator->Start();
	explosiveObject->SetOutline(true);
	explosiveObject->SetCanExplosive(true);
	HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST6");
	GetWorldTimerManager().SetTimer(questTimer, this, &APhase3::StartQuest, 2.0f, false);
	subTitleVoiceComp->SetSound(st7);
	subTitleVoiceComp->Play();
	blocker->OPEN();
}

void APhase3::RunningPhase3()
{

	if (explosiveObject && explosiveObject->GetIsExploded()&&!explodeDoOnce)
	{
		fence->Destroy(true);
		explodeDoOnce = true;
	}


	if (phase3EnterCP->IsBothEntered() && !enterDoOnce)
	{
		enterDoOnce = true;
		blocker->CLOSE();
	}

	if (phase3StartCP->IsAnyEntered() && !doOnce)
	{
		LOG_SCREEN("PHASE 3 START");
		subTitleVoiceComp->SetSound(st8);
		subTitleVoiceComp->Play();
		doOnce = true;
		BossProductionStart();
	}
	if (phase3EndCP->IsAnyEntered() && !endDoOnce && bossPhase->bIsBossPhaseRunning == false)
	{
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST8");
		subTitleVoiceComp->SetSound(st9);
		subTitleVoiceComp->Play();
		phase2to3PathIndicator->Stop();
		doOnce = true;
		endDoOnce = true;
	}

	if (doOnce && endDoOnce && !gameOverOnce)
	{
		gameOverOnce = true;
		HACKEDInGameManager->GetFadeInOutMaker()->FadeInOut(3.0f, 0.0f);
		GetWorldTimerManager().SetTimer(EndPhase3Timer, this, &APhase3::EndPhase3, 3.0f, false);
	}
}

void APhase3::EndPhase3()
{
	HACKEDInGameManager->GetEsper()->SetEnableInput(false, false, false);
	HACKEDInGameManager->GetCrusher()->SetEnableInput(false, false, false);
	HACKEDInGameManager->GetEsper()->SkillDisable(false, false, false, false, false, false, false);
	HACKEDInGameManager->GetCrusher()->SkillDisable(false, false, false, false, false, false, false);
	subTitleVoiceComp->SetSound(EndingCreditSound);
	subTitleVoiceComp->Play();
	HACKEDInGameManager->GetFadeInOutMaker()->ShowGameClear();
	GetWorldTimerManager().SetTimer(EndPhase3Timer, this, &APhase3::ReachDoorBackToLobby, 32.0f, false);
}

void APhase3::ReachDoorBackToLobby()
{
	InGameNetWork->EndGame();
}

void APhase3::BossProductionStart()
{
	RPC(NetBaseCP, APhase3, BossProductionStart, ENetRPCType::MULTICAST, true);

	HACKEDInGameManager->GetFadeInOutMaker()->FadeInOut(3.0f, 0.0f);
	GetWorldTimerManager().SetTimer(BossProductionTimer, this, &APhase3::BossProduction, 3.0f, false);
}

void APhase3::BossProduction()
{
	RPC(NetBaseCP, APhase3, BossProduction, ENetRPCType::MULTICAST, true);

	HACKEDInGameManager->GetFadeInOutMaker()->BossProduction();
	subTitleVoiceComp->SetSound(BossIntroSound);
	subTitleVoiceComp->Play();
	HACKEDInGameManager->GetEsper()->SetEnableInput(false, false, false);
	HACKEDInGameManager->GetCrusher()->SetEnableInput(false, false, false);
	HACKEDInGameManager->GetEsper()->SetActorLocation(BossPhaseLinkPos[0]->GetActorLocation());
	HACKEDInGameManager->GetCrusher()->SetActorLocation(BossPhaseLinkPos[1]->GetActorLocation());
	GetWorldTimerManager().SetTimer(BossProductionTimer, this, &APhase3::BossProductionEnd, 11.0f, false);

}

void APhase3::BossProductionEnd()
{
	HACKEDInGameManager->GetFadeInOutMaker()->BossProductionEnd();
	HACKEDInGameManager->GetEsper()->SetEnableInput(true, true, true);
	HACKEDInGameManager->GetCrusher()->SetEnableInput(true, true, true);
	HACKEDInGameManager->GetQuestManager()->StartQuest("MQ7");
	HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST9");
	subTitleVoiceComp->Stop();
	subTitleVoiceComp->SetSound(GoBossSound);
	subTitleVoiceComp->Play();
	bossPhase->StartBossPhase();
}

void APhase3::StartQuest()
{
	HACKEDInGameManager->GetQuestManager()->StartQuest("MQ5");
}
