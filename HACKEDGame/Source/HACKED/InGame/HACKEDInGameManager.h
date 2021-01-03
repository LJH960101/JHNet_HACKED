#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Core/Network/NetworkSystem.h"
#include "Core/JHNETGameInstance.h"
#include "Spawner/Spawner.h"
#include "Spawner/WaveSystem.h"
#include "InGame/Common/FadeInOutMaker.h"
#include "InGame/Common/SubtitleMaker.h"
#include "InGame/Network/InGameNetworkProcessor.h"
#include "InGame/Character/CRUSHER/HACKED_CRUSHER.h"
#include "InGame/Character/ESPER/HACKED_ESPER.h"
#include "InGame/Common/QuestManager.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "InGame/InGameManager.h"
#include "HACKEDInGameManager.generated.h"

UCLASS()
class HACKED_API AHACKEDInGameManager : public AInGameManager
{
	GENERATED_BODY()

public:
	AHACKEDInGameManager();

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintPure, Category = "Manager")
	ASpawner* GetSpawner();

	UFUNCTION(BlueprintPure, Category = "Manager")
	AWaveSystem* GetWaveSystem();

	UFUNCTION(BlueprintPure, Category = "Manager")
	AFadeInOutMaker* GetFadeInOutMaker();

	UFUNCTION(BlueprintPure, Category = "Manager")
	ASubtitleMaker* GetSubtitleMaker();

	UFUNCTION(BlueprintPure, Category = "Manager")
	AHACKED_CRUSHER* GetCrusher();

	UFUNCTION(BlueprintPure, Category = "Manager")
	AHACKED_ESPER* GetEsper();

	UFUNCTION(BlueprintPure, Category = "Manager")
	AQuestManager* GetQuestManager();

	// 크러셔와 에스퍼를 배열로 전달해준다.
	UFUNCTION(BlueprintPure, Category = "Manager")
	TArray<AHACKEDCharacter*> GetPlayers();

private:
	ASpawner* _spawner;
	AWaveSystem* _waveSystem;
	AFadeInOutMaker* _fadeOutMaker;
	ASubtitleMaker* _subtitleMaker;
	AHACKED_CRUSHER* _crusher;
	AHACKED_ESPER* _esper;
	AQuestManager* _questManager;
};
