// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "../Spawner/Spawner.h"
#include "../AI/HACKED_AI.h"
#include "EmptyZoneFinder.h"
#include "CheckPoint.h"
#include "Door.h"
#include "Phase_Lobby.generated.h"

const float CHARACTER_SELECT_WAIT_TIME = 5.0f;
const float CHARACTER_SELECT_SKIP_WAIT_TIME = 1.0f;

UCLASS()
class HACKED_API APhase_Lobby : public AActor
{
	GENERATED_BODY()

public:
	APhase_Lobby();

	UPROPERTY(EditAnywhere)
		ADoor* LobbyToLabDoor;
	UPROPERTY(EditAnywhere)
		ADoor* LabEnterDoor;
	uint32 enemyCount;
	UPROPERTY(EditAnywhere)
		AEmptyZoneFinder* enemyFinder;

	bool bIsDone;
	bool bDoOnce = false;

private:
	class ACharacterSelector* _characterSelector;
	bool _bOnPlayAI = false;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	void CalcEnemyCount();
	void AIAllRestart();
	void AIAllStop();

	FTimerHandle aiWaitTimer;
	FTimerHandle calcTimer;
};
