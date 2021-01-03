// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "WaveData.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "WaveSystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FWaveEndDelegate);

// WaveSystem State
UENUM(BlueprintType)
enum class EWaveState : uint8
{
	READY			UMETA(DisplayName = "Ready"),
	RUNNING			UMETA(DisplayName = "Running"),
	PAUSE			UMETA(DisplayName = "Pause"),
	SPAWN_END		UMETA(DisplayName = "SpawnEnd"),
	Max
};

// running wave's struct
USTRUCT()
struct FWave
{
	GENERATED_BODY()

	UPROPERTY()
	EHackedAIType aiType;

	UPROPERTY()
	FString spawnPosition;

	UPROPERTY()
	float spawnGab;

	UPROPERTY()
	int32 spawnGabMany;

	UPROPERTY()
	float spawnTimer;

	UPROPERTY()
	int32 spawnRemain;
};

UCLASS()
class HACKED_API AWaveSystem : public AActor
{
	GENERATED_BODY()
	
	const float FIRST_SPAWN_TIME_OF_WAVE = 2.0f;

public:
	// 웨이브가 끝났을때 멀티캐스팅 하는 델리게이트 입니다.
	FWaveEndDelegate waveEndDeleegate;

	// 스폰할 위치를 담는 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveSystem")
	TArray<AActor*> spawnPositions;

	// 웨이브를 시작합니다.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void StartWave(int32 waveNumber);

	// 현재 웨이브 스테이트를 리턴합니다.
	UFUNCTION(BlueprintPure, Category = "WaveSystem")
	EWaveState GetWaveState();

	// 웨이브를 중단합니다.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void StopWave();

	// 웨이브를 일지 정지합니다.
	// Start 상태에서만 동작합니다.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void PauseWave();

	// 웨이브를 재시작합니다.
	// Pause 상태에서만 작동합니다.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void ResumeWave();

	// 웨이브에서 일정 타입의 스폰을 막거나 킵니다.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void BlockSpawnType(EHackedAIType type, bool bIsBlock);

private:
	UPROPERTY()
	class UDataTable* _waveDataTable;

	int32 _currentWave;
	EWaveState _currentState;
	TArray<FWave> _currentWaveSpawnDatas;
	class ASpawner* _spawner;
	void ChangeState(EWaveState state);
	TMap<EHackedAIType, bool> _bIsblocks;
	class AHACKEDInGameManager* HACKEDInGameManager;

	RPC_FUNCTION(AWaveSystem, RPCChangeState, int);
	void RPCChangeState(int state);

	// 블락된(스폰을 멈춘) AI를 초기화 시킵니다.
	void InitBlockState();

public:	
	// Sets default values for this actor's properties
	AWaveSystem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	UNetworkBaseCP* NetBaseCP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
