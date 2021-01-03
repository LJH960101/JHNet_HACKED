// Fill out your copyright notice in the Description page of Project Settings.

#include "PhaseShelter.h"
#include "ElevatorDoor.h"
#include "CheckPoint.h"
#include "EmptyZoneFinder.h"
#include "InGame/AI/HACKED_AI.h"
#include "Core/HACKEDGameInstance.h"

APhaseShelter::APhaseShelter()
{
 	PrimaryActorTick.bCanEverTick = true;
	subTitleVoiceComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SUBTITLE SOUND COMP"));
}

void APhaseShelter::BeginPlay()
{
	Super::BeginPlay();
	BindHACKEDInGameManager(HACKEDInGameManager);
}
// Called every frame
void APhaseShelter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> enemies = shelterAICheck->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* hackedAI = Cast<AHACKED_AI>(iter);

			if (hackedAI->IsValidLowLevel())
			{
				++ShelterEnemyCount;
			}

			if (HACKEDInGameManager->GetEsper()->bIsNowOnTutorial)
			{
				AIAllStopTool::AIDisable(hackedAI);
			}
			else if (!(HACKEDInGameManager->GetEsper()->bIsNowOnTutorial))
			{
				if (bRestartOnce)
				{
					ShelterAIRestart();
					bRestartOnce = false;
				}			
			}
		}
	}
	if (ShelterEnemyCount == 0)
	{
		ShelterEnemyAllDie = true;
	}
	ShelterEnemyCount = 0;

	
	if (doorOpenCP->IsAnyEntered() && !doorDoOnce && ShelterEnemyAllDie)
	{
		doorDoOnce = true;
		elevatorDoor->OPEN();
		goodToGo = true;
		subTitleVoiceComp->Stop();
		
	}
	if (goodToGo && !exitDoOnce && exitShelterCP->IsAnyEntered() && ShelterEnemyAllDie)
	{
		exitDoOnce = true;
		HACKEDInGameManager->GetSubtitleMaker()->ShowSubtitleByCode("ST2");
		subTitleVoiceComp->SetSound(st2);
		subTitleVoiceComp->Play();
	}
}

void APhaseShelter::ShelterAIRestart()
{
	TArray<AActor*> enemies = shelterAICheck->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AHACKED_AI* hackedAI = Cast<AHACKED_AI>(iter);
			AIAllStopTool::AIRestart(hackedAI);
			
		}
	}
}

