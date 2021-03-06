// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 


#include "Phase1_1.h"
#include "InGame/HACKEDTool.h"
#include "Blueprint/UserWidget.h"
#include "../AI/HACKED_AI.h"


using namespace AIAllStopTool;

APhase1_1::APhase1_1()
{
	PrimaryActorTick.bCanEverTick = true;

	firstTargetHackingTime = 30.0f;
	secondTargetHackingTime = 30.0f;
	thirdTargetHackingTime = 30.0f;
	firstHackingRate = 0.0f;
	secondHackingRate = 0.0f;
	thirdHackingRate = 0.0f;
	DEBUG = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> hackingWG(TEXT("WidgetBlueprint'/Game/Blueprint/UI/InGame/Phase/HackingCompletionRate.HackingCompletionRate_C'"));
	if (hackingWG.Succeeded())
	{
		WG_Hacking_Class = hackingWG.Class;
	}
}

float APhase1_1::GetCurrentHackingRate()
{
	switch (phaseNumber)
	{
	case 1:
		_lastBackingRate = firstHackingRate / firstTargetHackingTime;
		break;
	case 2:
		_lastBackingRate = secondHackingRate / secondTargetHackingTime;
		break;
	case 3:
		_lastBackingRate = thirdHackingRate / thirdTargetHackingTime;
		break;
	default:
		LOG(Error, "Not exist phase!!!!!");
	}
	return _lastBackingRate;
}

void APhase1_1::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> first = emptyZoneFinders[0]->GetExistObject();
	TArray<AActor*> second = emptyZoneFinders[1]->GetExistObject();
	TArray<AActor*> third = emptyZoneFinders[2]->GetExistObject();
	TArray<AActor*> fourth = emptyZoneFinders[3]->GetExistObject();
	TArray<AActor*> fifth = emptyZoneFinders[4]->GetExistObject();
	for (AActor* iter : first)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			//LOG(Warning, "Group 1 : %s", *iter->GetName());
			AIAllStopTool::AIDisable((APawn*)iter);
		}
	}
	for (AActor* iter : second)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			//LOG(Warning, "Group 2 : %s", *iter->GetName());
			AIAllStopTool::AIDisable((APawn*)iter);
		}
	}
	for (AActor* iter : third)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			//LOG(Warning, "Group 3 : %s", *iter->GetName());
			AIAllStopTool::AIDisable((APawn*)iter);
		}
	}
	for (AActor* iter : fourth)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			//LOG(Warning, "Group 4 : %s", *iter->GetName());
			AIAllStopTool::AIDisable((APawn*)iter);
		}
	}
	for (AActor* iter : fifth)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			//LOG(Warning, "Group 5 : %s", *iter->GetName());
			AIAllStopTool::AIDisable((APawn*)iter);
		}
	}

	if (!wgHacking) wgHacking = CreateWidget<UUserWidget>(GetWorld(), WG_Hacking_Class);
}

void APhase1_1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (checkPoints[5]->IsAnyEntered())
	{
		Phase2PreEnemyStart();
	}

	if (checkPoints[6]->IsAnyEntered())
	{
		Phase3PreEnemyStart();
	}

	if (bIsPhase1Done)
	{
		Lab1End();
	}
	else
	{
		LabWaveTimers(DeltaTime);

		switch (phaseNumber)
		{
		case 0:
			Lab1End();
			break;
		case 1:
			Phase1Flow();
			break;
		case 2:
			Phase2Flow();
			break;
		case 3:
			Phase3Flow();
			break;
		default:
			LOG_SCREEN("SOMETHING FUCKED IN PHASE FLOW");
			break;
		}
	}
}

void APhase1_1::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (APlasmaDoor* door : plasmaDoors)
	{
		door->StartPlasmaBlocking();
	}

}

void APhase1_1::LabWaveTimers(float DeltaTime)
{
	switch (phaseNumber)
	{
	case 1:
		if (bIsPhase1Running && hackingPanels[0]->GetRunningState())
		{
			firstHackingRate += DeltaTime;
			//LOG_SCREEN("PHASE 1 hacking is progressing about %.2f", firstHackingRate);
		}
		break;
	case 2:
		if (bIsPhase2Running && hackingPanels[1]->GetRunningState())
		{
			secondHackingRate += DeltaTime;
			//LOG_SCREEN("PHASE 2 hacking is progressing about %.2f", secondHackingRate);
		}
		break;
	case 3:
		if (bIsPhase3Running && hackingPanels[2]->GetRunningState())
		{
			thirdHackingRate += DeltaTime;
			//LOG_SCREEN("PHASE 3 hacking is progressing about %.2f", thirdHackingRate);
		}
		break;
	}
}

void APhase1_1::Phase1Flow()
{
	//Enter Door Close
	if (DEBUG)
	{
		// Lab 1 Entered
		if (checkPoints[0]->IsAnyEntered())
		{
			PreparePhase1();
		}

		//When Player Enter Phase 1 Battery Sector
		if (checkPoints[1]->IsAnyEntered() && hackingPanels[0]->GetRunningState() && !bIsPhase1Running && !bPhase1Clear)
		{
			StartPhase1();
		}
		//Hacking
		else if (bIsPhase1Running && !bPhase1Clear)
		{
			RunningPhase1();
		}
		//End Hacking and Leave Phase 1
		else if (!bIsPhase1Running && bPhase1Clear && !hackingPanels[0]->GetRunningState())
		{
			EndPhase1();
		}
	}
	else
	{
		if (checkPoints[0]->IsBothEntered())
		{
			PreparePhase1();
		}

		//When Player Enter Phase 1 Battery Sector
		if (checkPoints[1]->IsBothEntered() && hackingPanels[0]->GetRunningState() && !bIsPhase1Running && !bPhase1Clear)
		{
			StartPhase1();
		}
		else if (bIsPhase1Running && !bPhase1Clear)
		{
			RunningPhase1();
		}
		else if (!bIsPhase1Running && bPhase1Clear && !hackingPanels[0]->GetRunningState())
		{
			EndPhase1();
		}
	}
}

void APhase1_1::Phase2Flow()
{
	if (DEBUG)
	{
		//Enter the Battery 2 Sector
		if (checkPoints[2]->IsAnyEntered() && !bJumpToPhase2)
		{
			PreparePhase2();
		}

		//When Player Enter Phase 2 Battery Sector
		if (checkPoints[2]->IsAnyEntered() && hackingPanels[1]->GetRunningState() && !bIsPhase2Running && !bPhase2Clear)
		{
			StartPhase2();
		}
		else if (bIsPhase2Running && !bPhase2Clear)
		{
			RunningPhase2();
		}
		else if (!bIsPhase2Running && bPhase2Clear && !hackingPanels[1]->GetRunningState())
		{
			EndPhase2();
		}
	}
	else
	{

		//Enter the Battery 2 Sector
		if (checkPoints[2]->IsBothEntered() && !bJumpToPhase2)
		{
			PreparePhase2();
		}

		//When Player Enter Phase 2 Battery Sector
		if (checkPoints[2]->IsBothEntered() && hackingPanels[1]->GetRunningState() && !bIsPhase2Running && !bPhase2Clear)
		{
			StartPhase2();
		}
		else if (bIsPhase2Running && !bPhase2Clear)
		{
			RunningPhase2();
		}
		else if (!bIsPhase2Running && bPhase2Clear && !hackingPanels[1]->GetRunningState())
		{
			EndPhase2();
		}
	}
}

void APhase1_1::Phase3Flow()
{
	if (DEBUG)
	{
		//Enter the Battery 3 Sector
		if (checkPoints[3]->IsAnyEntered() && !bJumpToPhase3)
		{
			PreparePhase3();
		}

		//When Player Enter Phase 3 Battery Sector
		if (checkPoints[3]->IsAnyEntered() && hackingPanels[2]->GetRunningState() && !bIsPhase3Running && !bPhase3Clear)
		{
			StartPhase3();
		}
		else if (bIsPhase3Running && !bPhase3Clear)
		{
			RunningPhase3();
		}
		else if (!bIsPhase3Running && bPhase3Clear && !hackingPanels[2]->GetRunningState())
		{
			EndPhase3();
		}
	}
	else
	{
		//Enter the Battery 3 Sector
		if (checkPoints[3]->IsBothEntered() && !bJumpToPhase3)
		{
			PreparePhase3();
		}

		//When Player Enter Phase 3 Battery Sector
		if (checkPoints[3]->IsBothEntered() && hackingPanels[2]->GetRunningState() && !bIsPhase3Running && !bPhase3Clear)
		{
			StartPhase3();
		}
		else if (bIsPhase3Running && !bPhase3Clear)
		{
			RunningPhase3();
		}
		else if (!bIsPhase3Running && bPhase3Clear && !hackingPanels[2]->GetRunningState())
		{
			EndPhase3();
		}
	}
}

void APhase1_1::PreparePhase1()
{
	//door close
	doors[0]->Close();
	doors[1]->Close();

	if (!bFirstDoorDoOnce)
	{
		plasmaDoors[0]->StopPlasmaBlocking();
		bFirstDoorDoOnce = true;
	}
	hackingPanels[0]->SetActiveState(true);

	TArray<AActor*>enemies = emptyZoneFinders[0]->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AIAllStopTool::AIRestart((APawn*)iter);
		}
	}

	TArray<AActor*>enemies2 = emptyZoneFinders[1]->GetExistObject();
	for (AActor* iter : enemies2)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AIAllStopTool::AIRestart((APawn*)iter);
		}
	}

}

void APhase1_1::StartPhase1()
{
	//phase is now running
	bIsPhase1Running = true;
	//activate plasmaDoor
	//start wave
	plasmaDoors[0]->StartPlasmaBlocking();
	plasmaDoors[1]->StartPlasmaBlocking();
	plasmaDoors[2]->StartPlasmaBlocking();
	waveSystem->StartWave(1);
	if (wgHacking || !wgHacking->IsInViewport()) wgHacking->AddToViewport();
}

void APhase1_1::RunningPhase1()
{
	//If the hacking was successfully done, and spawner did their job done. we're clear
	if (firstHackingRate >= firstTargetHackingTime &&
		waveSystem->GetWaveState() == EWaveState::SPAWN_END)
	{
		bIsPhase1Running = false;
		bPhase1Clear = true;
		hackingPanels[0]->EndHackingState();
	}
}

void APhase1_1::EndPhase1()
{
	//disable phase 1 -> phase 2 door
	plasmaDoors[1]->StopPlasmaBlocking();
	batteries[0]->Die();
	phaseNumber = 2;
	if (wgHacking || wgHacking->IsInViewport()) wgHacking->RemoveFromViewport();
}

void APhase1_1::Phase2PreEnemyStart()
{
	if (!bIsPhase2PreActivated)
	{
		if (DEBUG)
		{
			if (checkPoints[5]->IsAnyEntered() || phaseNumber == 1 || phaseNumber == 2)
			{
				TArray<AActor*>enemies = emptyZoneFinders[2]->GetExistObject();
				for (AActor* iter : enemies)
				{
					if (iter->IsA(AHACKED_AI::StaticClass()))
					{
						AIAllStopTool::AIRestart((APawn*)iter);
					}
				}
			}
			bIsPhase2PreActivated = true;
		}
		else
		{
			if (checkPoints[5]->IsBothEntered() || phaseNumber == 1 || phaseNumber == 2)
			{
				TArray<AActor*>enemies = emptyZoneFinders[2]->GetExistObject();
				for (AActor* iter : enemies)
				{
					if (iter->IsA(AHACKED_AI::StaticClass()))
					{
						AIAllStopTool::AIRestart((APawn*)iter);
					}
				}
			}
			bIsPhase2PreActivated = true;
		}
	}
}

void APhase1_1::PreparePhase2()
{
	//plasma door
	plasmaDoors[0]->StartPlasmaBlocking();
	plasmaDoors[1]->StartPlasmaBlocking();
	plasmaDoors[2]->StartPlasmaBlocking();
	bJumpToPhase2 = true;
	hackingPanels[1]->SetActiveState(true);
}

void APhase1_1::StartPhase2()
{
	bIsPhase2Running = true;
	waveSystem->StartWave(2);
	if (wgHacking || !wgHacking->IsInViewport()) wgHacking->AddToViewport();
}

void APhase1_1::RunningPhase2()
{
	if (secondHackingRate >= secondTargetHackingTime &&
		waveSystem->GetWaveState() == EWaveState::SPAWN_END)
	{
		bIsPhase2Running = false;
		bPhase2Clear = true;
		hackingPanels[1]->EndHackingState();
	}
}

void APhase1_1::EndPhase2()
{
	plasmaDoors[3]->StopPlasmaBlocking();
	batteries[1]->Die();
	phaseNumber = 3;
	if (wgHacking || wgHacking->IsInViewport()) wgHacking->RemoveFromViewport();
}

void APhase1_1::Phase3PreEnemyStart()
{
	if (!bIsPhase3PreActivated)
	{
		if (DEBUG)
		{
			if (checkPoints[6]->IsAnyEntered() || phaseNumber == 2 || phaseNumber == 3)
			{
				TArray<AActor*>enemies = emptyZoneFinders[3]->GetExistObject();
				for (AActor* iter : enemies)
				{
					if (iter->IsA(AHACKED_AI::StaticClass()))
					{
						AIAllStopTool::AIRestart((APawn*)iter);
					}
				}
				TArray<AActor*>enemies2 = emptyZoneFinders[4]->GetExistObject();
				for (AActor* iter : enemies2)
				{
					if (iter->IsA(AHACKED_AI::StaticClass()))
					{
						AIAllStopTool::AIRestart((APawn*)iter);
					}
				}
			}
			bIsPhase3PreActivated = true;
		}
		else
		{
			if (checkPoints[6]->IsBothEntered() || phaseNumber == 2 || phaseNumber == 3)
			{
				TArray<AActor*>enemies = emptyZoneFinders[3]->GetExistObject();
				for (AActor* iter : enemies)
				{
					if (iter->IsA(AHACKED_AI::StaticClass()))
					{
						AIAllStopTool::AIRestart((APawn*)iter);
					}
				}
				TArray<AActor*>enemies2 = emptyZoneFinders[4]->GetExistObject();
				for (AActor* iter : enemies2)
				{
					if (iter->IsA(AHACKED_AI::StaticClass()))
					{
						AIAllStopTool::AIRestart((APawn*)iter);
					}
				}
			}
			bIsPhase3PreActivated = true;
		}
	}
}

void APhase1_1::PreparePhase3()
{
	bJumpToPhase3 = true;
	hackingPanels[2]->SetActiveState(true);
}

void APhase1_1::StartPhase3()
{
	bIsPhase3Running = true;
	waveSystem->StartWave(3);
	plasmaDoors[3]->StartPlasmaBlocking();
	if (wgHacking || !wgHacking->IsInViewport()) wgHacking->AddToViewport();
}

void APhase1_1::RunningPhase3()
{
	if (thirdHackingRate >= thirdTargetHackingTime &&
		waveSystem->GetWaveState() == EWaveState::SPAWN_END)
	{
		bIsPhase3Running = false;
		bPhase3Clear = true;
		hackingPanels[2]->EndHackingState();
	}
}

void APhase1_1::EndPhase3()
{
	phaseNumber = 0;
	batteries[2]->Die();
	doors[2]->Open();
	if (wgHacking || wgHacking->IsInViewport()) wgHacking->RemoveFromViewport();
}

void APhase1_1::Lab1End()
{
	if (DEBUG)
	{
		if (checkPoints[4]->IsAnyEntered())
		{
			doors[2]->Close();
			bIsPhase1Done = true;
		}
		if (wgHacking || wgHacking->IsInViewport()) wgHacking->RemoveFromParent();
	}
	else
	{
		if (checkPoints[4]->IsBothEntered())
		{
			doors[2]->Close();
			bIsPhase1Done = true;
		}
		if (wgHacking || wgHacking->IsInViewport()) wgHacking->RemoveFromParent();
	}
}

