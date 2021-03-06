// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Phase_Lobby.h"
#include "AIController.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "InGame/HACKEDTool.h"
#include "BehaviorTree/BlackboardData.h"
#include "BrainComponent.h"
#include "InGame/Character/CharacterSelector/CharacterSelector.h"

using namespace AIAllStopTool;
// Sets default values
APhase_Lobby::APhase_Lobby()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	enemyCount = 1;

	bIsDone = false;
}

// Called when the game starts or when spawned
void APhase_Lobby::BeginPlay()
{
	Super::BeginPlay();

	AIAllStop();
	//GetWorldTimerManager().SetTimer(aiWaitTimer, this, &APhase_Lobby::AIAllRestart, CHARACTER_SELECT_WAIT_TIME, false);
	TArray<AActor*> outActors;
	GetWorldTimerManager().SetTimer(calcTimer, this, &APhase_Lobby::CalcEnemyCount, 1.0f, true);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterSelector::StaticClass(), outActors);
	if (outActors.Num() == 0) {
		_bOnPlayAI = true;
	}
	else {
		_characterSelector = Cast<ACharacterSelector>(outActors[0]);
	}
}

// Called every frame
void APhase_Lobby::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CHECK(LobbyToLabDoor && LabEnterDoor);
	if (!_bOnPlayAI && _characterSelector && _characterSelector->IsEndSelect()) {
		if (_characterSelector->bSkip) GetWorldTimerManager().SetTimer(aiWaitTimer, this, &APhase_Lobby::AIAllRestart, CHARACTER_SELECT_SKIP_WAIT_TIME, false);
		else GetWorldTimerManager().SetTimer(aiWaitTimer, this, &APhase_Lobby::AIAllRestart, CHARACTER_SELECT_WAIT_TIME, false);
		_bOnPlayAI = true;
	}
	if (bIsDone == false)
	{
		if (enemyCount <= 0)
		{
			GetWorldTimerManager().ClearTimer(calcTimer);
			LobbyToLabDoor->Open();
			LabEnterDoor->Open();
			bIsDone = true;
		}
		else
		{
			if (!bDoOnce)
			{
				LobbyToLabDoor->Close();
				LabEnterDoor->Close();
				bDoOnce = true;
			}
		}
	}
	else
	{
		//DO NOTHING
	}

}

void APhase_Lobby::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APhase_Lobby::CalcEnemyCount()
{
	enemyCount = 0;
	TArray<AActor*>enemies = enemyFinder->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			++enemyCount;
			//LOG(Warning, "%s", *iter->GetName());
		}	
	}
	//LOG_SCREEN("%d", enemyCount);
}

void APhase_Lobby::AIAllRestart()
{
	LOG_WARNING("AIAILLRESTART !!!");
	TArray<AActor*>enemies = enemyFinder->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			AIAllStopTool::AIRestart((APawn*)iter);
		}
	}
}

void APhase_Lobby::AIAllStop()
{
	TArray<AActor*>enemies = enemyFinder->GetExistObject();
	for (AActor* iter : enemies)
	{
		if (iter->IsA(AHACKED_AI::StaticClass()))
		{
			
			AIAllStopTool::AIDisable((APawn*)iter);
		}
	}
}
