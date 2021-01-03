// Fill out your copyright notice in the Description page of Project Settings.

#include "PhaseBoss.h"
#include "Blueprint/UserWidget.h"
#include "Ingame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "EmptyZoneFinder.h"
#include "InGame/HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"
#include "JHNET/InGame/Network/InGameNetworkProcessor.h"

APhaseBoss::APhaseBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	bossSoundComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AUDIO COMP"));

}

void APhaseBoss::BeginPlay()
{
	Super::BeginPlay();

	BindHACKEDInGameManager(HACKEDInGameManager);
	bossSoundComp->SetSound(bossSoundIntro);
	bossSoundComp->Stop();
	bossSoundComp->OnAudioFinished.AddDynamic(this, &APhaseBoss::PlayLoopSound);
}

void APhaseBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningBossPhase();
	
}

void APhaseBoss::StartBossPhase()
{
	bossSoundComp->FadeIn(soundFadeInTime);
	bIsBossPhaseRunning = true;
	TArray<AActor*>enemies = aiCheckZone->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* enemy = Cast<AHACKED_AI>(iter);

			AIAllStopTool::AIRestart(enemy);
			enemy->bIsAIImmortal = false;
			enemy->bPatrolCheck = true;
			enemy->GetMesh()->SetVisibility(true);
		}
	}
	boss->AddHPWidgetToViewport();
	HACKEDInGameManager->GetQuestManager()->CompleteSubQuest("SQ7");
}

void APhaseBoss::RunningBossPhase()
{
	if (!bIsBossPhaseRunning)
		return;

	if (boss->IsDie())
	{
		if (!doOnce)
		{
			boss->RemoveHPWidgetToViewport();
			bossSoundComp->FadeOut(soundFadeInTime, 0.0f);
			doOnce = true;
			bBossAlive = false;
			HACKEDInGameManager->GetQuestManager()->StartQuest("MQ6");
			HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST11");
			bossSoundComp->SetSound(GoToDoorAgain);
			bossSoundComp->Play();
			bIsBossPhaseRunning = false;
		}
	}
	else
	{
		bBossAlive = true;
	}
}

void APhaseBoss::PlayLoopSound()
{
	//보스 뒤지면 알아서 FadeOut
	if (!bBossAlive)
		return;

	//인트로 사운드 끝나면 루프사운드로 갈아끼움
	if (bossSoundComp->Sound == bossSoundIntro)
	{
		bossSoundComp->Stop();
		bossSoundComp->SetSound(bossSoundLoop);
		bossSoundComp->Play();
	}
	//루프사운드 끝나면 그냥 다시 시작시킴
	else if (bossSoundComp->Sound == bossSoundLoop)
	{
		bossSoundComp->Play();
	}
	else {}
}
