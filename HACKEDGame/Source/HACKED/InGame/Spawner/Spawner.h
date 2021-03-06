// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "WaveData.h"
#include "Array.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "InGame/Interface/Networkable.h"
#include "Spawner.generated.h"

USTRUCT()
struct FEntry
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<AActor*> Entries;
};

typedef TMap<EHackedAIType, FEntry> FSpawnMap;

UCLASS()
class HACKED_API ASpawner : public AActor, public INetworkable
{
	GENERATED_BODY()

	friend class AWaveSystem;
public:
	// 기본 오브젝트 양.
	UPROPERTY(EditAnywhere)
	int DEFAULT_OBJECT_BY_AI = 20;

	// 오브젝트 풀이 꽉 찼을경우 추가해줄 양.
	UPROPERTY(EditAnywhere)
	int OBJECT_ADD_AMOUNT = 30;

	// 오브젝트 풀 최대 치.
	UPROPERTY(EditAnywhere)
	int MAX_OBJECT = 200;

	// AI를 스폰합니다.
	// 클라이언트에게도 스폰한 정보를 전송합니다.
	// 마스터 클라이언트만 사용하길 바랍니다.
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* Spawn(EHackedAIType type, FTransform transform);

	RPC_FUNCTION(ASpawner, MASTERSpawnObject, int, FVector, int)
		UFUNCTION(BlueprintCallable, Category = "Spawner")
		void MASTERSpawnObject(int type, FVector location, int many);

	RPC_FUNCTION(ASpawner, RPCSpawnObjectByName, FString, FVector, int)
		UFUNCTION(BlueprintCallable, Category = "Spawner")
		void RPCSpawnObjectByName(FString targetName, FVector location, int type);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void DestroyObject(AActor* destroyTarget);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	class UNetworkBaseCP* NetBaseCP;

	// Object를 끄는 로직.
	// **Spawner와 독립적인 로직입니다.
	static void ActiveObject(AActor* actor, bool isOn);

	// 이미 존재하는 액터인지 조사하고,
	// 아니라면 추가 시킨다.
	// 새로이 추가되었다면, (Spawner에서 스폰된것이 아니라면)
	// true를 리턴한다.
	bool CheckAndAddAlreadyExistObject(AActor* actor);

	// 재접속 관련 처리입니다.
	virtual void Reconnected() override;

	// 재접속 관련 처리입니다.
	RPC_FUNCTION(ASpawner, RPCRefreshSpawner, FString)
		void RPCRefreshSpawner(FString buf);

	// Sets default values for this actor's properties
	ASpawner();

private:
	int _spawnCounter = 0;
	FSpawnMap _spawnedObject;
	FSpawnMap _notSpawndObject;
	TMap<EHackedAIType, UClass*> _spawnClasses;

	// 모든 AI를 해제합니다.
	// 재접속에서 사용합니다.
	void _DisableAll();

	// AI를 정렬합니다.
	// 재접속에서 사용합니다.
	void _SortEntries();

	void _SpawnObjects(EHackedAIType type, FTransform transform, int many = 1);
	void _SpawnObjects(EHackedAIType type, FVector location, int many = 1);

	// AI가 존재하는지 확인합니다.
	// 월드에 존재하는 AI BeginPlay시, 강제로 오브젝트풀에 넣기위해서 사용합니다.
	bool _ContainObject(AActor* actor);

	// 오브젝트 풀을 확장합니다.
	// retval : 성공 여부
	bool _ResizePool(EHackedAIType type);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
};