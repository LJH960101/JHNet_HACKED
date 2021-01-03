// Fill out your copyright notice in the Description page of Project Settings.
#include "WaveSystemLimiter.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Spawner/WaveSystem.h"
#include "InGame/HACKEDInGameManager.h"
#include "Components/BoxComponent.h"
#include "InGame/HACKEDTool.h"
#include "Core/HACKEDGameInstance.h"

// Sets default values
AWaveSystemLimiter::AWaveSystemLimiter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		_spawnClasses.Add((EHackedAIType)i, GetAIClassByType((EHackedAIType)i));
	}
	_bOnStopWave = false;

	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		_overlapActors.FindOrAdd((EHackedAIType)i);
	}
}

void AWaveSystemLimiter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AWaveSystemLimiter::OnBoxBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &AWaveSystemLimiter::OnBoxEndOverlap);
}

bool AWaveSystemLimiter::IsFull(EHackedAIType type)
{
	if (maxAiNumbs.Contains(type) && _overlapActors.Contains(type) && _overlapActors[type].Num() >= maxAiNumbs[type]) return true;
	else return false;
}

bool AWaveSystemLimiter::IsSomethingFull()
{
	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		if (IsFull((EHackedAIType)i)) return true;
	}
	return false;
}

TArray<AActor*> AWaveSystemLimiter::GetExistObjects(EHackedAIType type)
{
	return _overlapActors[type];
}

int AWaveSystemLimiter::GetExistObjectNum(EHackedAIType type)
{
	return _overlapActors[type].Num();
}

// Called when the game starts or when spawned
void AWaveSystemLimiter::BeginPlay()
{
	Super::BeginPlay();

	BindHACKEDInGameManager(HACKEDInGameManager);

	// Get WaveSystem
	_waveSystem = HACKEDInGameManager->GetWaveSystem();
}

// Called every frame
void AWaveSystemLimiter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CHECK(_waveSystem);

	// 스포너가 실행중이라면 꽉찼는지 확인한다.
	if (_waveSystem->GetWaveState() == EWaveState::RUNNING)
	{
		// 해당 타입을 체크하고 블락 시킨다.
		for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
			if (IsFull((EHackedAIType)i)) {
				_waveSystem->BlockSpawnType((EHackedAIType)i, true);
			}
			else {
				_waveSystem->BlockSpawnType((EHackedAIType)i, false);
			}
		}
	}
}

void AWaveSystemLimiter::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AHACKED_AI::StaticClass())) {
		_AddActor(OtherActor);
	}
}

void AWaveSystemLimiter::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AHACKED_AI::StaticClass())) {
		_RemoveActor(OtherActor);
	}
}

void AWaveSystemLimiter::_AddActor(AActor* actor)
{
	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		if (actor->IsA(GetAIClassByType((EHackedAIType)i))) {
			_overlapActors[(EHackedAIType)i].Push(actor);
		}
	}
}

void AWaveSystemLimiter::_RemoveActor(AActor* actor)
{
	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		if (actor->IsA(GetAIClassByType((EHackedAIType)i))) {
			if(_overlapActors[(EHackedAIType)i].Contains(actor)) _overlapActors[(EHackedAIType)i].Remove(actor);
		}
	}
}
