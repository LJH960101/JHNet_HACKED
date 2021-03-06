// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Spawner.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/SR_ET/SecurityRobot_ET.h"
#include "InGame/AI/SR_LS/SecurityRobot_LS.h"
#include "InGame/AI/SR_SLS/SecurityRobot_SLS.h"
#include "AIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BrainComponent.h"
#include "InGame/HACKEDTool.h"
#include "NetworkModule/NetworkInfo.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));

	for (uint8 i = 0; i < (uint8)EHackedAIType::MAX; ++i) {
		_spawnClasses.Add((EHackedAIType)i, GetAIClassByType((EHackedAIType)i));
	}
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	BindRPCFunction(NetBaseCP, ASpawner, RPCRefreshSpawner);
	BindRPCFunction(NetBaseCP, ASpawner, MASTERSpawnObject);
	BindRPCFunction(NetBaseCP, ASpawner, RPCSpawnObjectByName);
}

void ASpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Create Object and hide them.
	for (int aiType = 0; aiType < (int)EHackedAIType::MAX; ++aiType) {
		FEntry& aiSet = _notSpawndObject.FindOrAdd((EHackedAIType)aiType);
		_spawnedObject.FindOrAdd((EHackedAIType)aiType);

		FActorSpawnParameters spawnParms;
		FTransform spawnTransform = FTransform::Identity;
		spawnTransform.SetLocation(FVector(1000000.0f, 1000000.0f, 1000000.0f));
		spawnParms.bNoFail = true;
		spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		for (int spawnNumb = 0; spawnNumb < DEFAULT_OBJECT_BY_AI; ++spawnNumb) {
			FString newName = FString::Printf(TEXT("Spawned%dtype%d"), aiType, _spawnCounter++);
			spawnParms.Name = FName(*newName);
			AActor* newActor = GetWorld()->SpawnActor(_spawnClasses[(EHackedAIType)aiType], &spawnTransform, spawnParms);
#if WITH_EDITOR
			newActor->SetActorLabel(newName);
#endif
			if (newActor == nullptr) {
				LOG(Error, "Can't spawn new actor!!!");
				continue;
			}
			ActiveObject(newActor, false);
			aiSet.Entries.Add(newActor);
		}
	}
	_SortEntries();
}

AActor* ASpawner::Spawn(EHackedAIType type, FTransform transform)
{
	if (type == EHackedAIType::MAX) {
		LOG(Error, "Wrong type !!!");
		return nullptr;		
	}

	TArray<AActor*>& entry = _notSpawndObject.Find(type)->Entries;
	if (entry.Num() <= 0) {
		// 비었으면 해당 몬스터의 오브젝트 풀을 늘려줍니다.
		if(!_ResizePool(type)) return nullptr;
	}

	AActor* spawnTargetActor = entry.Pop();
	spawnTargetActor->SetActorTransform(transform);
	ActiveObject(spawnTargetActor, true);
	_spawnedObject.Find(type)->Entries.Push(spawnTargetActor);

	AHACKED_AI* ai = Cast<AHACKED_AI>(spawnTargetActor);
	if (ai) ai->OnSpawn();

	// 이후 클라이언트들에도 스폰을 통보합니다.
	FString aiName = spawnTargetActor->GetName();
	RPCSpawnObjectByName(aiName, transform.GetLocation(), (int)type);

	return spawnTargetActor;
}

void ASpawner::_SpawnObjects(EHackedAIType type, FTransform transform, int many)
{
	for (int i = 0; i < many; ++i) {
		Spawn(type, transform);
	}
}

void ASpawner::_SpawnObjects(EHackedAIType type, FVector location, int many)
{
	FTransform transform = FTransform::Identity;
	transform.SetLocation(location);
	_SpawnObjects(type, transform, many);
}

bool ASpawner::_ContainObject(AActor* actor)
{
	for (auto i : _spawnedObject) {
		for (AActor* j : i.Value.Entries) {
			if (j == actor) {
				return true;
			}
		}
	}
	for (auto i : _notSpawndObject) {
		for (AActor* j : i.Value.Entries) {
			if (j == actor) {
				return true;
			}
		}
	}

	return false;
}

bool ASpawner::_ResizePool(EHackedAIType type)
{
	TArray<AActor*>& entry = _notSpawndObject.Find(type)->Entries;
	if (entry.Num() >= MAX_OBJECT) {
		LOG_ERROR("Failed to ResizePool. Too many object in entry. MAX_OBJECT=%d, currentNum=%d", MAX_OBJECT, entry.Num());
		return false;
	}

	FActorSpawnParameters spawnParms;
	FTransform spawnTransform = FTransform::Identity;
	spawnTransform.SetLocation(FVector(1000000.0f, 1000000.0f, 1000000.0f));
	spawnParms.bNoFail = true;
	spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int spawnNumb = 0; spawnNumb < OBJECT_ADD_AMOUNT; ++spawnNumb) {
		FString newName = FString::Printf(TEXT("Spawned%dtype%d"), (int)type, _spawnCounter++);
		spawnParms.Name = FName(*newName);
		AActor* newActor = GetWorld()->SpawnActor(_spawnClasses[type], &spawnTransform, spawnParms);
#if WITH_EDITOR
		newActor->SetActorLabel(newName);
#endif
		if (newActor == nullptr) {
			LOG(Error, "Can't spawn new actor!!!");
			continue;
		}
		ActiveObject(newActor, false);
		entry.Add(newActor);
	}
	return true;
}

void ASpawner::MASTERSpawnObject(int type, FVector location, int many)
{
	RPC(NetBaseCP, ASpawner, MASTERSpawnObject, ENetRPCType::MASTER, true, type, location, many);
	_SpawnObjects((EHackedAIType)type, location, many);
}

void ASpawner::RPCSpawnObjectByName(FString targetName, FVector location, int type)
{
	RPC(NetBaseCP, ASpawner, RPCSpawnObjectByName, ENetRPCType::MULTICAST, true, targetName, location, type);
	if (NetBaseCP->IsMaster()) return;

	if (type < 0 || (EHackedAIType)type >= EHackedAIType::MAX) return;

	// 5회만 시도해본다.
	for (int tryCount = 0; tryCount < 5; ++tryCount) {
		FString name;

		// 스폰되지 않은 풀에 있는지 조사
		TArray<AActor*>& notSpawnedEntries = _notSpawndObject.Find((EHackedAIType)type)->Entries;
		for (AActor* i : notSpawnedEntries) {
			i->GetName(name);
			if (name == targetName) {
				i->SetActorLocation(location);
				ActiveObject(i, true);
				notSpawnedEntries.Remove(i);
				_spawnedObject.Find((EHackedAIType)type)->Entries.Push(i);

				AHACKED_AI* ai = Cast<AHACKED_AI>(i);
				if (ai) ai->OnSpawn();
				return;
			}
		}

		// 스폰되어 있는 풀에 있는지 조사
		// 이미 있다면 위치만 갱신
		TArray<AActor*>& spawnedEntries = _spawnedObject.Find((EHackedAIType)type)->Entries;
		for (AActor* i : spawnedEntries) {
			i->GetName(name);
			if (name == targetName) {
				i->SetActorLocation(location);

				AHACKED_AI* ai = Cast<AHACKED_AI>(i);
				if (ai) ai->OnSpawn();
				return;
			}
		}

		// 풀에 존재하지 않는다면 풀을 늘려서 다시 시도한다.
		if (!_ResizePool((EHackedAIType)type)) break;
	}
	LOG(Error, "Can't spawn name : %s, %d", *targetName, type);
}

void ASpawner::DestroyObject(AActor* destroyTarget)
{
	for (auto i : _spawnedObject) {
		for (AActor* j : i.Value.Entries) {
			if (j == destroyTarget) {
				ActiveObject(j, false);
				i.Value.Entries.Remove(j);
				_notSpawndObject[i.Key].Entries.Push(destroyTarget);
				return;
			}
		}
	}
	LOG(Warning, "Can't find object...");
}

void ASpawner::_DisableAll()
{
	for (int i = 0; i < (int)EHackedAIType::MAX; ++i) {
		TArray<AActor*>& entries = _spawnedObject.FindOrAdd((EHackedAIType)i).Entries;
		while (entries.Num() != 0)
		{
			AActor* actor = entries.Pop();
			ActiveObject(actor, false);
			_notSpawndObject[(EHackedAIType)i].Entries.Push(actor);
		}
	}
}

void ASpawner::_SortEntries()
{
	// BeginPlay/재접속 시 1회 수행되는 선택정렬.
	// 액터의 이름순으로 정렬해서 동기화를 유지한다.
	for (int type = 0; type < (int)EHackedAIType::MAX; ++type) {
		TArray<AActor*>& spawnedEntries = _spawnedObject.FindOrAdd((EHackedAIType)type).Entries;
		for (int i = 0; i < spawnedEntries.Num(); ++i) {
			for (int j = i + 1; j < spawnedEntries.Num(); ++j) {
				if (spawnedEntries[i]->GetName() > spawnedEntries[j]->GetName()) {
					AActor* temp = spawnedEntries[i];
					spawnedEntries[i] = spawnedEntries[j];
					spawnedEntries[j] = temp;
				}
			}
		}

		TArray<AActor*>& notSpawnedEntries = _notSpawndObject.FindOrAdd((EHackedAIType)type).Entries;
		for (int i = 0; i < notSpawnedEntries.Num(); ++i) {
			for (int j = i + 1; j < notSpawnedEntries.Num(); ++j) {
				if (notSpawnedEntries[i]->GetName() > notSpawnedEntries[j]->GetName()) {
					AActor* temp = notSpawnedEntries[i];
					notSpawnedEntries[i] = notSpawnedEntries[j];
					notSpawnedEntries[j] = temp;
				}
			}
		}
	}
}

void ASpawner::ActiveObject(AActor* actor, bool isOn)
{
	actor->SetActorHiddenInGame(!isOn);
	actor->SetActorEnableCollision(isOn);
	actor->SetActorTickEnabled(isOn);
	TArray<UActorComponent*> Components;
	actor->GetComponents(Components);
	for (int32 CompIdx = 0; CompIdx < Components.Num(); CompIdx++) {
		Components[CompIdx]->SetComponentTickEnabled(isOn);
	}

	auto pawn = Cast<APawn>(actor);
	if (pawn != nullptr) {
		if(isOn) AIAllStopTool::AIRestart(pawn);
		else AIAllStopTool::AIDisable(pawn);
	}
	AHACKED_AI* hackedAi = Cast<AHACKED_AI>(actor);
	if (hackedAi) hackedAi->SetDie(!isOn);
}

bool ASpawner::CheckAndAddAlreadyExistObject(AActor* actor)
{
	// 이미 존재하는지 검사한다.
	if (_ContainObject(actor)) return false;

	// Find by Class
	for (int j_type = 0; j_type < (int)EHackedAIType::MAX; ++j_type) {
		if (actor->IsA(_spawnClasses[(EHackedAIType)j_type])) {
			// Add actor to Set
			if (actor->bHidden) {
				FEntry& aiSet = _notSpawndObject.FindOrAdd((EHackedAIType)j_type);
				aiSet.Entries.Add(actor);
			}
			else {
				FEntry& aiSet = _spawnedObject.FindOrAdd((EHackedAIType)j_type);
				aiSet.Entries.Add(actor);
			}
			return true;
		}
	}
	LOG(Error, "Not exist ai type...");
	return true;
}

struct FInt3 {
	FInt3(int x, int y, int z) : X(x), Y(y), Z(z) {}
	int X;
	int Y;
	int Z;
};
void ASpawner::Reconnected()
{

	// 모든 스폰된 몬스터를 [타입, 위치]로 보낸다.
	int aiLen = 0;
	TArray<EHackedAIType> types;
	TArray<FInt3> locations;

	for (int i_type = 0; i_type < (int)EHackedAIType::MAX; ++i_type) {
		// 스폰된 몬스터의 갯수를 센다.
		TArray<AActor*>& entries = _spawnedObject.FindOrAdd((EHackedAIType)i_type).Entries;
		aiLen += entries.Num();
		for (int j_actor = 0; j_actor < entries.Num(); ++j_actor) {
			AActor* actor = entries[j_actor];
			if (!actor) continue;

			// 타입을 담는다.
			types.Add((EHackedAIType)i_type);

			// 위치를 담는다.
			FVector location = actor->GetActorLocation();
			FInt3 newLocation = FInt3(location.X, location.Y, location.Z);
			locations.Add(newLocation);
		}
	}

	// 보낼 버퍼를 만든다.
	// 갯수-타입,x,y,z@타입,x,y,z ....
	FString result = FString::Printf(TEXT("%d-"), aiLen);
	for (int i = 0; i < aiLen; ++i) {
		if(i == aiLen - 1) result.Append(FString::Printf(TEXT("%d,%d,%d,%d"), types[i], locations[i].X, locations[i].Y, locations[i].Z));
		else result.Append(FString::Printf(TEXT("%d,%d,%d,%d@"), types[i], locations[i].X, locations[i].Y, locations[i].Z));
	}

	RPCRefreshSpawner(result);
}

void ASpawner::RPCRefreshSpawner(FString buf)
{
	RPC(NetBaseCP, ASpawner, RPCRefreshSpawner, ENetRPCType::MULTICAST, true, buf);

	_DisableAll();
	_SortEntries();

	// 버퍼를 파싱한다.
	// 갯수-타입,x,y,z@타입,x,y,z ....

	int32 aiLen = FCString::Atoi(*buf);

	// 갯수는 버퍼에서 삭제해버린다.
	for (int i = 0; i < buf.Len(); ++i) {
		if (buf[i] == '-') {
			buf.RemoveAt(0, i+1);
			break;
		}
	}

	TArray<FString> parsedActorsString; // type,x,y,z

	// 갯수가 맞지않다면 리턴.
	if (aiLen != buf.ParseIntoArray(parsedActorsString, TEXT("@"), true)) {
		LOG(Error, "Not matched buf...");
		return;
	}
	for (int i = 0; i < aiLen; ++i) {
		TArray<FString> parsedActorString; // type x y z
		if (4 != parsedActorsString[i].ParseIntoArray(parsedActorString, TEXT(","), true)) {
			LOG(Error, "Not matched buf...");
			continue;
		}

		EHackedAIType type = (EHackedAIType)(FCString::Atoi(*parsedActorString[0]));
		FVector newLocation = FVector::ZeroVector;
		newLocation.X = FCString::Atoi(*parsedActorString[1]);
		newLocation.Y = FCString::Atoi(*parsedActorString[2]);
		newLocation.Z = FCString::Atoi(*parsedActorString[3]);

		FTransform newTransform = FTransform::Identity;
		newTransform.SetLocation(newLocation);
		Spawn(type, newTransform);
	}
	LOG(Warning, "End Refresh Spawner");
}