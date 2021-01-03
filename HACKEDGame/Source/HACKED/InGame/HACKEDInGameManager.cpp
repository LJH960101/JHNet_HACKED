// Fill out your copyright notice in the Description page of Project Settings.

#include "HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"

using namespace ActorFinderTool;

// Sets default values
AHACKEDInGameManager::AHACKEDInGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AHACKEDInGameManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Get Objects
	if(!_spawner) GetSpawner();
	if(!_waveSystem) GetWaveSystem();
	if(!_fadeOutMaker) GetFadeInOutMaker();
	if(!_subtitleMaker) GetSubtitleMaker();
	if(!_crusher) GetCrusher();
	if(!_esper) GetEsper();
	if(!_questManager) GetQuestManager();
}

ASpawner* AHACKEDInGameManager::GetSpawner()
{
	if (!_spawner) _spawner = CreateOrGetObject<ASpawner>(GetWorld());
	return _spawner;
}

AWaveSystem* AHACKEDInGameManager::GetWaveSystem()
{
	if (!_waveSystem) _waveSystem = CreateOrGetObject<AWaveSystem>(GetWorld());
	return _waveSystem;
}

AFadeInOutMaker* AHACKEDInGameManager::GetFadeInOutMaker()
{
	if (!_fadeOutMaker) _fadeOutMaker = CreateOrGetObject<AFadeInOutMaker>(GetWorld());
	return _fadeOutMaker;
}

ASubtitleMaker* AHACKEDInGameManager::GetSubtitleMaker()
{
	if (!_subtitleMaker) _subtitleMaker = CreateOrGetObject<ASubtitleMaker>(GetWorld());
	return _subtitleMaker;
}

AHACKED_CRUSHER* AHACKEDInGameManager::GetCrusher()
{
	UClass* crusherClass = nullptr;
	crusherClass = LoadClass<AHACKED_CRUSHER>(NULL, TEXT("Blueprint'/Game/Blueprint/InGame/Character/Crusher/BP_Crusher.BP_Crusher_C'"));
	if (!_crusher) _crusher = CreateOrGetObject<AHACKED_CRUSHER>(GetWorld(), crusherClass);
	return _crusher;
}

AHACKED_ESPER* AHACKEDInGameManager::GetEsper()
{
	UClass* esperClass = nullptr;
	esperClass = LoadClass<AHACKED_ESPER>(NULL, TEXT("Blueprint'/Game/Blueprint/InGame/Character/Esper/BP_Esper.BP_Esper_C'"));
	if (!_esper) _esper = CreateOrGetObject<AHACKED_ESPER>(GetWorld(), esperClass);
	return _esper;
}

AQuestManager* AHACKEDInGameManager::GetQuestManager()
{
	if (!_questManager) _questManager = CreateOrGetObject<AQuestManager>(GetWorld());
	return _questManager;
}

TArray<AHACKEDCharacter*> AHACKEDInGameManager::GetPlayers()
{
	TArray<AHACKEDCharacter*> newArray;
	if(_crusher) newArray.Add(GetCrusher());
	if (_esper) newArray.Add(GetEsper());
	return newArray;
}