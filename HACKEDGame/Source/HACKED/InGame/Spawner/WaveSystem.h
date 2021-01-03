// Copyright��2019 by ������ī��. All Data cannot be copied without permission. 

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
	// ���̺갡 �������� ��Ƽĳ���� �ϴ� ��������Ʈ �Դϴ�.
	FWaveEndDelegate waveEndDeleegate;

	// ������ ��ġ�� ��� �����Դϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveSystem")
	TArray<AActor*> spawnPositions;

	// ���̺긦 �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void StartWave(int32 waveNumber);

	// ���� ���̺� ������Ʈ�� �����մϴ�.
	UFUNCTION(BlueprintPure, Category = "WaveSystem")
	EWaveState GetWaveState();

	// ���̺긦 �ߴ��մϴ�.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void StopWave();

	// ���̺긦 ���� �����մϴ�.
	// Start ���¿����� �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void PauseWave();

	// ���̺긦 ������մϴ�.
	// Pause ���¿����� �۵��մϴ�.
	UFUNCTION(BlueprintCallable, Category = "WaveSystem")
	void ResumeWave();

	// ���̺꿡�� ���� Ÿ���� ������ ���ų� ŵ�ϴ�.
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

	// �����(������ ����) AI�� �ʱ�ȭ ��ŵ�ϴ�.
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
