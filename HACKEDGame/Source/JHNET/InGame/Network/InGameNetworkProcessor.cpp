// Fill out your copyright notice in the Description page of Project Settings.
//#define DEBUG_RPC
#include "InGameNetworkProcessor.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "InGame/InGameManager.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/NetworkTool.h"
#include "NetworkModule/NetworkInfo.h"
#include "Blueprint/UserWidget.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/PacketStruct.h"
#include "InGame/Interface/Networkable.h"
#include "InGame/JHNETTool.h"

using namespace NetworkTool;
using namespace std;
using namespace JHNETSerializer;

// Sets default values
AInGameNetworkProcessor::AInGameNetworkProcessor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NetBaseCP = CreateDefaultSubobject<UNetworkBaseCP>(TEXT("Network Base"));
	_slotAndName.Init(TTuple<bool,FString>(false,""), MAX_PLAYER);
	
	static ConstructorHelpers::FClassFinder<UUserWidget> DisConnectWidget(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_Disconnect.WB_Disconnect_C'"));
	if (DisConnectWidget.Succeeded()) {
		_disconnectWGClass = DisConnectWidget.Class;
	}

	/*TSubclassOf<AActor> dummyActorClass;
	GetClassAsset(dummyActorClass, AActor, "Blueprint'/Game/Blueprint/DummyActor.DummyActor_C'");
	classes.Add(ENetSpawnType::DUMMY, dummyActorClass);*/
}

// FString name, int slot
void AInGameNetworkProcessor::_RPCSetSlotSteamName(FString name, int slot)
{
	RPC(NetBaseCP, AInGameNetworkProcessor, _RPCSetSlotSteamName, ENetRPCType::MULTICAST, true, name, slot);
	if(slot >=0 && slot <MAX_PLAYER){
		_slotAndName[slot] = TTuple<bool, FString>(true, name);
	}
}

void AInGameNetworkProcessor::_RPCNetworkSpawn(uint64 currentSpawnCount, int type, FTransform transform)
{
	RPC(NetBaseCP, AInGameNetworkProcessor, _RPCNetworkSpawn, ENetRPCType::MULTICAST, true, currentSpawnCount, type, transform);

	FString objectID = FString::Printf(TEXT("NetworkSpawn%llu"), currentSpawnCount);
	AActor* retval;
	UClass* spawnClass = GetClassByType((ENetSpawnType)type);
	JHNET_CHECK(spawnClass);

	FActorSpawnParameters spawnParms;
	spawnParms.bNoFail = true;
	spawnParms.Name = *(objectID);
	spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	retval = GetWorld()->SpawnActor(spawnClass, &transform, spawnParms);

	if (!retval) {
		JHNET_LOG(Error, "Can't spawn actor!!!");
		return;
	}
	retval->SetActorTransform(transform);

	UActorComponent* actCmp = retval->GetComponentByClass(UNetworkBaseCP::StaticClass());
	if (actCmp) {
		UNetworkBaseCP* netBaseCmp = Cast<UNetworkBaseCP>(actCmp);
		netBaseCmp->SetObjectIDByNetworkID(objectID);
	}
}

void AInGameNetworkProcessor::_SetSlotSteamName()
{
	_RPCSetSlotSteamName(GetMyName(), GetCurrentSlot());
}

void AInGameNetworkProcessor::_OnRefreshSlot(bool isMaster)
{
	for (auto i : _networkActors) {
		AActor* actor = i.Value;
		if (actor &&
			actor->GetClass()->ImplementsInterface(UNetworkable::StaticClass())) {
			auto networkableActor = Cast<INetworkable>(actor);
			if (networkableActor) networkableActor->OnSlotChange(isMaster);
		}
	}
}

void AInGameNetworkProcessor::OnConnected()
{
	// Not OnSteam But OnServer, Try force Join to game room.
	if (_networkSystem && _networkSystem->OnServer() && !_networkSystem->OnSteam()) {
		EnterToGame_DEBUG();
		JHNET_LOG(Warning, "Try Force Game Join...");
	}
}

AInGameNetworkProcessor::~AInGameNetworkProcessor()
{
	_savedList.Empty();
}

void AInGameNetworkProcessor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	_gameInstance = GetGameInstance();
	JHNET_CHECK(_gameInstance);
	_networkSystem = _gameInstance->GetNetworkSystem();
	JHNET_CHECK(_networkSystem);
	InGameManager = _gameInstance->GetInGameManager();
	JHNET_CHECK(InGameManager);
}

void AInGameNetworkProcessor::BeginPlay()
{
	Super::BeginPlay();
	BindRPCFunction(NetBaseCP, AInGameNetworkProcessor, _RPCSetSlotSteamName);
	BindRPCFunction(NetBaseCP, AInGameNetworkProcessor, _RPCNetworkSpawn);
	if (_networkSystem && _networkSystem->OnServer()) {
		OnRefreshRoom();
	}
	GetWorldTimerManager().SetTimer(_reconnectJHNET_CHECKTimerHandle, this, &AInGameNetworkProcessor::_ReconnectJHNET_CHECK, 0.1f, false);

	if (GetNetworkSystem()->GetReconnectMode()) {
		_ReconnectProcess();
	}
}

void AInGameNetworkProcessor::OnRefreshRoom()
{
	Super::OnRefreshRoom();
	_SetSlotSteamName();
}

void AInGameNetworkProcessor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Object에 도달하지 않는 메세지를 다시 처리한다.
	for (int i = 0; i < _savedList.Num(); ++i) {
		FReplicationData& data = _savedList[i];
		if (JHNET_CHECKSavedObject(data, DeltaTime)) {
			if (data.delayTime >= MAX_SAVED_TIME) {
				JHNET_LOG(Error, "data removed by timeout... : object : %s, usage : %s", *(data.objectHandle), *(data.usageHandle));
			}
			_savedList.RemoveAt(i);
			--i;
			continue;
		}
	}
}

bool AInGameNetworkProcessor::JHNET_CHECKSavedObject(FReplicationData& data, float deltaTime)
{
	data.delayTime += deltaTime;
	if (data.delayTime >= MAX_SAVED_TIME) return true;

	return SendToObject(data);
}

bool AInGameNetworkProcessor::SendToObject(FReplicationData & data)
{
	AActor** targetObject = _networkActors.Find(data.objectHandle);
	if (targetObject == nullptr || (*targetObject) == nullptr || !((*targetObject)->HasActorBegunPlay())) {
		//JHNET_LOG(Error, "Not exist object... : Object Name %s, UsageHandle %s", *(data.objectHandle), *(data.usageHandle));
		return false;
	}
	// Get network base cp
	auto cp = (*targetObject)->GetComponentByClass(UNetworkBaseCP::StaticClass());
	if (cp == nullptr) {
		JHNET_LOG(Error, "Can't cast to NetBaseCP");
		return false;
	}
	UNetworkBaseCP* networkCP = Cast<UNetworkBaseCP>(cp);
	if (networkCP == nullptr) {
		JHNET_LOG(Error, "Can't cast to NetBaseCP");
		return false;
	}

	if (!networkCP->OnSync()) return false;
	if(data.isRPC) networkCP->ExecutedNetFunc(data.usageHandle, data.len, data.buf);
	else networkCP->RecvSyncVar(data.usageHandle, data.len, data.buf);
	return true;
}

void AInGameNetworkProcessor::_Disconnect(int32 slot)
{
	if(slot < 0 || slot >= MAX_PLAYER) return;
	if (!_disconnectWG) _disconnectWG = CreateWidget<UUserWidget>(GetWorld(), _disconnectWGClass);
	if (_disconnectWG && !_disconnectWG->IsInViewport()) _disconnectWG->AddToViewport();
	_slotAndName[slot] = TTuple<bool, FString>(false, _slotAndName[slot].Get<1>());
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	for (auto i : _networkActors) {
		AActor* actor = i.Value;
		if (actor &&
			actor->GetClass()->ImplementsInterface(UNetworkable::StaticClass())) {
			auto networkableActor = Cast<INetworkable>(actor);
			if (networkableActor) networkableActor->Disconnected();
		}
	}
}

void AInGameNetworkProcessor::_Reconnected(int32 slot)
{
	if(slot < 0 || slot >= MAX_PLAYER) return;
	_slotAndName[slot] = TTuple<bool, FString>(true, _slotAndName[slot].Get<1>());
	if(GetPlayerNumbs() == MAX_PLAYER){
		// All user has been reconnected.
		if (_disconnectWG && _disconnectWG->IsInViewport()) _disconnectWG->RemoveFromParent();
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	}
	for (auto i : _networkActors) {
		AActor* actor = i.Value;
		if (actor &&
			actor->HasActorBegunPlay() &&
			actor->GetClass()->ImplementsInterface(UNetworkable::StaticClass())) {
			auto networkableActor = Cast<INetworkable>(actor);
			if(networkableActor) networkableActor->Reconnected();
		}
	}
}

void AInGameNetworkProcessor::_ReconnectProcess()
{
	if (_networkSystem) _networkSystem->bNeedToResync = false;
	for (auto i : _networkActors) {
		AActor* actor = i.Value;
		if (actor &&
			actor->GetClass()->ImplementsInterface(UNetworkable::StaticClass())) {
			auto networkableActor = Cast<INetworkable>(actor);
			if (networkableActor) networkableActor->Reconnect();
		}
	}
	GetNetworkSystem()->SetReconnectMode(false);
}

void AInGameNetworkProcessor::_ReconnectJHNET_CHECK()
{
	if (_networkSystem && _networkSystem->bNeedToResync) {
		_ReconnectProcess();
	}
}

UNetworkSystem * AInGameNetworkProcessor::GetNetworkSystem()
{
	return _networkSystem;
}

bool AInGameNetworkProcessor::AddObject(AActor * networkActor)
{
	UActorComponent* cp = networkActor->GetComponentByClass(UNetworkBaseCP::StaticClass());
	if (!cp) return false;
	UNetworkBaseCP* netCP = Cast<UNetworkBaseCP>(cp);
	if (!netCP) return false;

	AActor** mapActor = _networkActors.Find(netCP->GetObjectIDByString());
	if (mapActor != nullptr && *mapActor != nullptr) return false;
	_networkActors.Add(netCP->GetObjectIDByString(), networkActor);
	return true;
}

void AInGameNetworkProcessor::RemoveObject(AActor * networkActor)
{
	UActorComponent* cp = networkActor->GetComponentByClass(UNetworkBaseCP::StaticClass());
	if (!cp) return;
	UNetworkBaseCP* netCP = Cast<UNetworkBaseCP>(cp);
	if (!netCP) return;

	AActor** mapActor = _networkActors.Find(netCP->GetObjectIDByString());
	if (mapActor != nullptr && *mapActor == networkActor) {
		_networkActors.Remove(netCP->GetObjectIDByString());
	}
}

void AInGameNetworkProcessor::GetSyncDataForce(AActor* networkActor)
{
	JHNET_CHECK(networkActor);

	// Object에 도달하지 않는 메세지를 다시 처리한다.
	
	for (int i = 0; i < _savedList.Num(); ++i) {
		FReplicationData& data = _savedList[i];
		AActor** targetActor = _networkActors.Find(data.objectHandle);
		if (!targetActor) {
			JHNET_LOG_ERROR("targetActor == nullptr");
		}
		if (targetActor && *targetActor == networkActor) {
			if (JHNET_CHECKSavedObject(data, 0.0f)) {
				if (data.delayTime >= MAX_SAVED_TIME) {
					JHNET_LOG(Error, "data removed by timeout... : object : %s, usage : %s", *(data.objectHandle), *(data.usageHandle));
				}
				_savedList.RemoveAt(i);
				--i;
				continue;
			}
		}
	}
}

int32 AInGameNetworkProcessor::GetCurrentSlot()
{
	if(_networkSystem) return _networkSystem->GetCurrentSlot();
	else return 0;
}

FString AInGameNetworkProcessor::GetMyName()
{
	if(_networkSystem) return _networkSystem->GetMySteamName();
	else return FString("NoName");
}

FString AInGameNetworkProcessor::GetSlotName(int32 slot)
{
	if(slot >= 0 && slot < MAX_PLAYER) return _slotAndName[slot].Get<1>();
	else return FString("Not exist slot.");
}

int AInGameNetworkProcessor::GetPlayerNumbs()
{
	int count = 0;
	for(int i=0; i<MAX_PLAYER; ++i){
		if(GetNetworkSystem()->GetSteamID(i) != 0)  ++count;
	}
	return count;
}

void AInGameNetworkProcessor::EndGame()
{
	FC_End_Game cEndGame;

	Send((FPacketStruct*)&cEndGame, true);
}

void AInGameNetworkProcessor::RefreshSlot()
{
	_OnRefreshSlot(IsMaster());
}

bool AInGameNetworkProcessor::IsMaster()
{
	return GetCurrentSlot() == 0;
}

bool AInGameNetworkProcessor::IsSlave()
{
	return !IsMaster();
}

uint64 AInGameNetworkProcessor::GetHashNumber(const FString& actorName)
{
	uint32_t hash, i, len = actorName.Len();
	for (hash = i = 0; i < len; ++i)
	{
		hash += actorName[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	uint64 res = static_cast<uint64>(hash);
	while (res >= SPAWN_COUNT_START) {
		res /= 2;
	}
	return res;
}

void AInGameNetworkProcessor::EnterToGame_DEBUG()
{
	FC_Debug_GameStart cDebugGameStart;
	Send((FPacketStruct*)&cDebugGameStart, true);
}

void AInGameNetworkProcessor::SetSteamID_DEBUG(const FString & steamID)
{
	_networkSystem->SetSteamID_DUBGE(FStringToUINT64(steamID));
}

void AInGameNetworkProcessor::NetworkSpawn(ENetSpawnType type, FTransform transform)
{
	if (!IsMaster()) {
		JHNET_LOG_SCREEN("NetworkSpawn only work on master.");
		return;
	}
	uint64 currentSpawnCount = _spawnCount++;
	_RPCNetworkSpawn(currentSpawnCount, static_cast<int32>(type), transform);
}

UClass * AInGameNetworkProcessor::GetClassByType(const ENetSpawnType& type)
{
	if (classes.Find(type) == nullptr) {
		JHNET_LOG(Error, "Not registered Type. type : %d", (int)type);
		return nullptr;
	}
	else return classes[type];
}

void AInGameNetworkProcessor::RecvProc(FReciveData& data)
{
	Super::RecvProc(data);

	int cursor = 0, currentCursor;
	while (cursor < data.len) {
		JHNET_CHECK(cursor >= 0 && cursor <= PACKET_BUFSIZE);
		int bufLen = IntDeserialize(data.buf, &cursor);
		JHNET_CHECK(bufLen >= 0 && bufLen <= PACKET_BUFSIZE);

		currentCursor = cursor;
		FPacketStruct newPacket;
		newPacket.Deserialize(data.buf, &currentCursor);
		
		switch (newPacket.type)
		{
		case EMessageType::S_InGame_RPC:
		{
			InGame_RPC(data, cursor);
			break;
		}
		case EMessageType::S_InGame_SyncVar:
		{
			InGame_SyncVar(data, cursor);
			break;
		}
		case EMessageType::S_Disconnect_Slot:
		{
			InGame_DisconnectSlot(data, cursor);
			break;
		}
		case EMessageType::S_Reconnect_Slot:
		{
			InGame_ReconnectSlot(data, cursor);
			break;
		}
		case EMessageType::S_End_Game:
		{
			EndGame(data, cursor);
			return;
		}
		case EMessageType::S_Reconnect:
		{
			Reconnect(data, cursor);
			break;
		}
		case EMessageType::S_Room_Info:
		{
			Room_Info(data, cursor);
			break;
		}
		default:
		{
			cursor += bufLen;
			break;
		}
		}
	}
}

void AInGameNetworkProcessor::InGame_RPC(FReciveData& data, int& cursor)
{
	JHNET_CHECK(GetNetworkSystem());

	FS_InGame_RPC sInGameRPC;
	sInGameRPC.Deserialize(data.buf, &cursor);

	// Parse Data
	FString objectID = JHNETSerializer::BufToFString(sInGameRPC.objectHandle);
	FString functionHandle = JHNETSerializer::BufToFString(sInGameRPC.idenHandle);
	int32 len = sInGameRPC.argBuf.len;
	JHNET_CHECK(len >= 0 && len < PACKET_BUFSIZE);

	char* buf = GetNetworkSystem()->bufObjectPool->PopObject();
	memcpy(buf, sInGameRPC.argBuf.buf, len);

	FReplicationData newData;
	newData.buf = buf;
	newData.delayTime = 0.0f;
	newData.isRPC = true;
	newData.len = len;
	newData.objectHandle = objectID;
	newData.usageHandle = functionHandle;

#ifdef DEBUG_RPC
	JHNET_LOG_WARNING("RPC : %s %s", *objectID, *functionHandle);
#endif

	if (!SendToObject(newData)) {
		_savedList.Add(newData);
	}
	else {
		GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
	}
}

void AInGameNetworkProcessor::InGame_SyncVar(FReciveData& data, int& cursor)
{
	JHNET_CHECK(GetNetworkSystem());

	FS_InGame_SyncVar sInGameSyncVar;
	sInGameSyncVar.Deserialize(data.buf, &cursor);

	// Parse Data
	FString objectID = JHNETSerializer::BufToFString(sInGameSyncVar.objectHandle);
	FString handle = JHNETSerializer::BufToFString(sInGameSyncVar.idenHandle);
	int32 len = sInGameSyncVar.argBuf.len;
	JHNET_CHECK(len >= 0 && len < PACKET_BUFSIZE);

	char* buf = GetNetworkSystem()->bufObjectPool->PopObject();
	memcpy(buf, sInGameSyncVar.argBuf.buf, len);

	FReplicationData newData;
	newData.buf = buf;
	newData.delayTime = 0.0f;
	newData.isRPC = false;
	newData.len = len;
	newData.objectHandle = objectID;
	newData.usageHandle = handle;

#ifdef DEBUG_RPC
	JHNET_LOG_WARNING("RPC : %s %s", *objectID, *handle);
#endif

	if (!SendToObject(newData)) {
		_savedList.Add(newData);
	}
	else {
		GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
	}
}

void AInGameNetworkProcessor::InGame_DisconnectSlot(FReciveData& data, int& cursor)
{
	FS_Disconnect_Slot sDisconnectSlot;
	sDisconnectSlot.Deserialize(data.buf, &cursor);

	int32& targetSlot = sDisconnectSlot.slot;
	_Disconnect(targetSlot);
}

void AInGameNetworkProcessor::InGame_ReconnectSlot(FReciveData& data, int& cursor)
{
	FS_Reconnect_Slot sReconnectSlot;
	sReconnectSlot.Deserialize(data.buf, &cursor);

	int32& targetSlot = sReconnectSlot.slot;
	_Reconnected(targetSlot);
}

void AInGameNetworkProcessor::EndGame(FReciveData& data, int& cursor)
{
	FS_End_Game sEndGame;
	sEndGame.Deserialize(data.buf, &cursor);

	GetNetworkSystem()->SetInGameManager(nullptr);
	if (!fadeScreen->IsInViewport()) fadeScreen->AddToViewport(fadeZIndex);
	if (fadeScreen) fadeScreen->FadeOut(3.0f);

	GetWorld()->GetTimerManager().SetTimer(_endGameTimer, this, &AInGameNetworkProcessor::OnEndGame, 3.2f, false);
}

void AInGameNetworkProcessor::OnEndGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Lobby");
}

void AInGameNetworkProcessor::Reconnect(FReciveData& data, int& cursor)
{
	FS_Reconnect sReconnect;
	sReconnect.Deserialize(data.buf, &cursor);

	for (int i = 0; i < MAX_PLAYER; ++i) {
		GetNetworkSystem()->SetSteamID(i, sReconnect.slot[i]);
	}

	GetNetworkSystem()->SetReconnectMode(true);
	_ReconnectProcess();
}

void AInGameNetworkProcessor::Room_Info(FReciveData& data, int& cursor)
{
	FS_Room_Info sRoomInfo;
	sRoomInfo.Deserialize(data.buf, &cursor);

	if (_disconnectWG && _disconnectWG->IsInViewport()) _disconnectWG->RemoveFromParent();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	if(GetWorld()->GetFirstPlayerController()) GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
}
