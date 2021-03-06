// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "WaveSystem.h"
#include "Spawner.h"
#include "InGame/HACKEDInGameManager.h"
#include "InGame/HACKEDTool.h"

// Sets default values
AWaveSystem::AWaveSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_SPAWN_DATA(TEXT("DataTable'/Game/Data/DT_Spawn.DT_Spawn'"));
	CHECK(DT_SPAWN_DATA.Succeeded());
	_waveDataTable = DT_SPAWN_DATA.Object;
	CHECK(_waveDataTable->GetRowMap().Num() > 0);

	InitBlockState();

	_currentState = EWaveState::READY;
}

void AWaveSystem::ChangeState(EWaveState state)
{
	_currentState = state;
	RPCChangeState((int)state);
}

void AWaveSystem::RPCChangeState(int state)
{
	RPC(NetBaseCP, AWaveSystem, RPCChangeState, ENetRPCType::MULTICAST, true, state);
	if (state < 0 || state >= (int)EWaveState::Max) {
		LOG(Error, "Wrong state arg.");
		return;
	}
	_currentState = (EWaveState)state;
}

void AWaveSystem::InitBlockState()
{
	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		_bIsblocks.Emplace((EHackedAIType)i, false);
	}
}

void AWaveSystem::StartWave(int32 waveNumber)
{
	if (NetBaseCP->IsSlave()) return;

	ChangeState(EWaveState::RUNNING);
	InitBlockState();
	_currentWave = waveNumber;
	_currentWaveSpawnDatas.Empty();
	int rowNum = _waveDataTable->GetRowMap().Num();
	for (int i = 1; i <= rowNum; ++i) {
		FWaveData* waveData = _waveDataTable->FindRow<FWaveData>(*FString::FromInt(i), TEXT(""));
		if (waveData == nullptr) {
			LOG_S(Error);
			continue;
		}
		if (waveData->waveNumb == waveNumber && waveData->spawnMany != 0 && waveData->spawnGabMany != 0) {
			FWave newSpawnData;
			newSpawnData.aiType = waveData->aiType;
			newSpawnData.spawnPosition = waveData->spawnPosition;
			newSpawnData.spawnGab = waveData->spawnGab;
			newSpawnData.spawnGabMany = waveData->spawnGabMany;
			newSpawnData.spawnTimer = FIRST_SPAWN_TIME_OF_WAVE;
			newSpawnData.spawnRemain = waveData->spawnMany;
			_currentWaveSpawnDatas.Add(newSpawnData);
		}
	}
	LOG(Warning, "Wave %d Start!!! (%d)", waveNumber, _currentWaveSpawnDatas.Num());
}

EWaveState AWaveSystem::GetWaveState()
{
	return _currentState;
}

void AWaveSystem::StopWave()
{
	ChangeState(EWaveState::SPAWN_END);
	_currentWaveSpawnDatas.Empty();
}

void AWaveSystem::PauseWave()
{
	if (GetWaveState() != EWaveState::RUNNING) return;
	ChangeState(EWaveState::PAUSE);
}

void AWaveSystem::ResumeWave()
{
	if (GetWaveState() != EWaveState::PAUSE) return;
	ChangeState(EWaveState::RUNNING);
}

void AWaveSystem::BlockSpawnType(EHackedAIType type, bool bIsBlock)
{
	if (NetBaseCP->IsSlave()) return;
	_bIsblocks.Emplace(type, bIsBlock);
}

// Called when the game starts or when spawned
void AWaveSystem::BeginPlay()
{
	Super::BeginPlay();

	BindHACKEDInGameManager(HACKEDInGameManager);
	BindRPCFunction(NetBaseCP, AWaveSystem, RPCChangeState);

	// Find Spawner
	_spawner = HACKEDInGameManager->GetSpawner();
	
	if (spawnPositions.Num() == 0) {
		LOG_SCREEN("WaveSystem must be have spawn positions!!!!");
		LOG(Error, "WaveSystem must be have spawn positions!!!!");
	}
}

// Called every frame
void AWaveSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CHECK(_spawner != nullptr);
	// 웨이브는 마스터에서만 처리한다.
	if (NetBaseCP->IsSlave()) return;
	if (_currentState == EWaveState::RUNNING) {
		CHECK(spawnPositions.Num() > 0);
		for (int i = _currentWaveSpawnDatas.Num() - 1; i >= 0; --i) {
			FWave& wave = _currentWaveSpawnDatas[i];
			// 블락된 타입이면 넘어간다.
			if (_bIsblocks[wave.aiType]) continue;

			wave.spawnTimer -= DeltaTime;
			// time to spawn
			if (wave.spawnTimer <= 0.0f) {
				wave.spawnTimer += wave.spawnGab;
				int spawnMany = wave.spawnGabMany;
				if (wave.spawnGabMany >= wave.spawnRemain) spawnMany = wave.spawnRemain;

				CHECK(spawnPositions.Num() != 0);
				if (wave.spawnPosition == "") {
					_spawner->MASTERSpawnObject((int)wave.aiType, 
						spawnPositions[FMath::RandRange(0, spawnPositions.Num() - 1)]->GetActorLocation(), 
						spawnMany);
				}
				else {
					TArray<FString> splitedPosition;
					wave.spawnPosition.ParseIntoArray(splitedPosition, TEXT("-"), true);
					CHECK(splitedPosition.Num() != 0);

					int splitedSpawnMany = spawnMany / splitedPosition.Num();
					int remainSpawnMany = spawnMany % splitedPosition.Num(); // remains object are spawn on first position
					for (int j = 0; j < splitedPosition.Num(); ++j) {
						int spawnPos = FCString::Atoi(*splitedPosition[j]);
						if (spawnPos >= spawnPositions.Num()) {
							LOG(Error, "There's no position of number %d", spawnPos);
							continue;
						}
						if (j == 0) _spawner->MASTERSpawnObject((int)wave.aiType, spawnPositions[spawnPos]->GetActorLocation(), splitedSpawnMany + remainSpawnMany);
						else _spawner->MASTERSpawnObject((int)wave.aiType, spawnPositions[spawnPos]->GetActorLocation(), splitedSpawnMany);
					}
				}

				// Remove if spawn end.
				wave.spawnRemain -= spawnMany;
				if (wave.spawnRemain <= 0) {
					_currentWaveSpawnDatas.RemoveAt(i);
				}
			}
		}
		if (_currentWaveSpawnDatas.Num() == 0) {
			ChangeState(EWaveState::SPAWN_END);
			waveEndDeleegate.Broadcast();
		}
	}
}