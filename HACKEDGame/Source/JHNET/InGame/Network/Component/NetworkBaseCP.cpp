// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "NetworkBaseCP.h"
#include "InGame/InGameManager.h"
#include "../InGameNetworkProcessor.h"
#include "Core/JHNETGameInstance.h"
#include "NetworkModule/Serializer.h"
#include "Core/Network/NetworkSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include <functional>
#include <memory>

using namespace std;
using namespace JHNETSerializer;

// Sets default values for this component's properties
UNetworkBaseCP::UNetworkBaseCP()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	_bOnSync = true;
}

void UNetworkBaseCP::EnableSync(bool isOn)
{
	_bOnSync = isOn;
	if (isOn) {
		GetOwner()->GetWorldTimerManager().ClearTimer(SyncStopTimerHandle);
		GetInGameNetworkProcessor()->GetSyncDataForce(GetOwner());
	}
}

bool UNetworkBaseCP::IsEditor(){
	return GetWorld()->GetGameState()->PlayerArray.Num() >= 2;
}

bool UNetworkBaseCP::IsSingle(){
	return GetPlayerNums() <= 1;
}

int UNetworkBaseCP::GetPlayerNums(){
	if(_inGameNetworkProcessor) return _inGameNetworkProcessor->GetPlayerNumbs();
	else return 1;
}

void UNetworkBaseCP::PlayStopSyncTime(float time)
{
	GetOwner()->GetWorldTimerManager().SetTimer(SyncStopTimerHandle, this, &UNetworkBaseCP::_OnStopSyncEnd, time, false);
	EnableSync(false);
}

void UNetworkBaseCP::__RPCSetAuthority(int slot)
{
	RPC(this, UNetworkBaseCP, __RPCSetAuthority, ENetRPCType::MULTICAST, true, slot);
	isPlayerAuthority = true;
	_authorityId = slot;

	if (_authorityId == GetCurrentSlot()) {
		auto pawn = Cast<APawn>(GetOwner());
		if (pawn != nullptr) {
			auto playerController = GetWorld()->GetFirstPlayerController();
			if (playerController != nullptr) {
				playerController->Possess(pawn);
			}
		}
	}
	// Set other player's actor gravity to zero
	else {
		auto pawn = Cast<APawn>(GetOwner());
		if (pawn != nullptr) {
			auto character = Cast<ACharacter>(pawn);
			if(character) character->GetCharacterMovement()->GravityScale = 0.0f;

			FActorSpawnParameters spawnParms;
			FTransform spawnTransform = FTransform::Identity;
			spawnParms.bNoFail = true;
			spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			APlayerController* controller = Cast<APlayerController>(GetWorld()->SpawnActor(APlayerController::StaticClass(), &spawnTransform, spawnParms));
			controller->Possess(pawn);
		}
	}
}

int32 UNetworkBaseCP::GetCurrentSlot()
{
	if (_inGameNetworkProcessor)  return _inGameNetworkProcessor->GetNetworkSystem()->GetCurrentSlot();
	else return 0;
}

int32 UNetworkBaseCP::GetCurrentAuthority()
{
	return _authorityId;
}

AInGameNetworkProcessor* UNetworkBaseCP::GetInGameNetworkProcessor()
{
	return _inGameNetworkProcessor;
}

void UNetworkBaseCP::__RPCNetworkDestroy()
{
	RPC(this, UNetworkBaseCP, __RPCNetworkDestroy, ENetRPCType::MULTICAST, true);
	AActor* actor = GetOwner();
	if(actor)
		actor->Destroy();
}

void UNetworkBaseCP::SetAuthority(int32 slot)
{
	if (_inGameNetworkProcessor && !_inGameNetworkProcessor->IsMaster()) JHNET_LOG(Error, "Only Master can give authority.");

	__RPCSetAuthority(slot);
}

void UNetworkBaseCP::NetworkDestroy()
{
	if (_inGameNetworkProcessor && !_inGameNetworkProcessor->IsMaster()) JHNET_LOG(Error, "Only Master can network destroy.");

	__RPCNetworkDestroy();
}

UNetworkSystem* UNetworkBaseCP::GetNetworkSystem(){
	if(_inGameNetworkProcessor) return _inGameNetworkProcessor->GetNetworkSystem();
	else return nullptr;
}

void UNetworkBaseCP::SetObjectIDByNetworkID(const FString& id)
{
	JHNET_CHECK(_inGameNetworkProcessor);
	if (_isRegistered) {
		_inGameNetworkProcessor->RemoveObject(GetOwner());
	}
	_objectID = id;
	_isAuto = false;
	if (!_inGameNetworkProcessor->AddObject(GetOwner())) {
		JHNET_LOG(Error, "Can't add object.....");
	}
}

bool UNetworkBaseCP::HasAuthority()
{
	if (!_inGameNetworkProcessor || _inGameNetworkProcessor->GetNetworkSystem() == nullptr) {
		if(_authorityId == 0) return true;
		else return false;
	}

	if(_inGameNetworkProcessor->GetNetworkSystem()->GetCurrentSlot() == _authorityId) return true;
	else return false;
}

bool UNetworkBaseCP::IsMaster()
{
	if (_inGameNetworkProcessor) return _inGameNetworkProcessor->IsMaster();
	else return true;
}

bool UNetworkBaseCP::IsSlave()
{
	if (_inGameNetworkProcessor) return _inGameNetworkProcessor->IsSlave();
	else return false;
}

bool UNetworkBaseCP::OnServer()
{
	if (_inGameNetworkProcessor)  return _inGameNetworkProcessor->GetNetworkSystem()->OnServer();
	else return false;
}

bool UNetworkBaseCP::IsPlayerAuthority()
{
	return isPlayerAuthority;
}

FString UNetworkBaseCP::GetObjectIDByString()
{
	return _objectID;
}

void UNetworkBaseCP::_OnStopSyncEnd()
{
	EnableSync(true);
}

void UNetworkBaseCP::SetObjectIDByAuto()
{
	if (_inGameNetworkProcessor) {
		_objectID = GetOwner()->GetName();
	}
	else _objectID = FString(TEXT("None"));
}

// Called when the game starts
void UNetworkBaseCP::BeginPlay()
{
	Super::BeginPlay();

	// Get NetworkProcessor
	AActor* owner = GetOwner();
	UJHNETGameInstance* gameInstance = Cast<UJHNETGameInstance>(owner->GetGameInstance());
	JHNET_CHECK(gameInstance);
	AInGameManager* inGameManager = Cast<AInGameManager>(gameInstance->GetNetworkSystem()->GetInGameManager());
	JHNET_CHECK(inGameManager);
	_inGameNetworkProcessor = inGameManager->GetNetworkProcessor();
	JHNET_CHECK(_inGameNetworkProcessor);

	// Register object ID
	if (!_isRegistered)
	{
		bool bOnSuccess;
		if(_isAuto) SetObjectIDByAuto();
		bOnSuccess = _inGameNetworkProcessor->AddObject(GetOwner());
		if (!bOnSuccess) {
			FString name = "";
			GetOwner()->GetName(name);
			JHNET_LOG(Error, "%s : Object ID have been overlapped! Please change the ID.", *name);
			return;
		}
		_isRegistered = true;
	}

	BindRPCFunction(this, UNetworkBaseCP, __RPCSetAuthority);
	BindRPCFunction(this, UNetworkBaseCP, __RPCNetworkDestroy);
}

void UNetworkBaseCP::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (_inGameNetworkProcessor) {
		_inGameNetworkProcessor->RemoveObject(GetOwner());
	}
}

bool UNetworkBaseCP::BindSyncVar(const FString& handle, int32 len, void * adress)
{
	if (JHNET_CHECKAlreadyUseSyncVar(handle)) {
		JHNET_LOG(Error, "Already exist handle!! name : %s, handle : %s", *(GetOwner()->GetName()), *handle);
		return false;
	}
	_syncVars.Add(handle, make_tuple(len,adress));
	return true;
}

FRPCFunction * UNetworkBaseCP::CreateBindableSyncVarDelegate(const FString& handle, int32 len, void * adress)
{
	if (JHNET_CHECKAlreadyUseSyncVar(handle)) {
		JHNET_LOG(Error, "Already exist handle!! name : %s, handle : %s", *(GetOwner()->GetName()), *handle);
		return false;
	}
	FRPCFunction* newFunc = new FRPCFunction();
	_hookSyncVars.Add(handle, make_tuple(len, adress, newFunc));
	return newFunc;
}

bool UNetworkBaseCP::SetSyncVar(const FString& handle, int len, char * arg, bool isReliable)
{
	JHNET_CHECK(_inGameNetworkProcessor && _inGameNetworkProcessor->GetNetworkSystem(), false);

	if (!HasAuthority()){
		return false;
	}
	if (!JHNET_CHECKAlreadyUseSyncVar(handle)) {
		JHNET_LOG(Error, "Not exist handle. Object = %s, Handle = %s", *GetObjectIDByString(), *handle);
		return false;
	}

	auto syncVarTuple = GetSyncVarTuple(handle);
	if (get<1>(syncVarTuple) == nullptr) {
		JHNET_LOG(Error, "Not exist handle. Object = %s, Handle = %s", *GetObjectIDByString(), *handle);
		return false;
	}
	if (get<0>(syncVarTuple) > len) {
		JHNET_LOG(Error, "Not valid argType. origin : %d, your : %d", get<0>(syncVarTuple), len);
		return false;
	}
	memcpy(get<1>(syncVarTuple), arg, len);

	char* buf = _inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->PopObject();
	JHNETSerializer::Serialize(arg, buf, len);
	SendSyncVarToServer(handle, len, buf, isReliable);
	_inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->ReturnObject(buf);

	auto synvarDelegate = get<2>(syncVarTuple);
	if (synvarDelegate != nullptr && synvarDelegate->IsBound()) {
		synvarDelegate->Execute(len, arg);
	}
	return true;
}

void UNetworkBaseCP::RecvSyncVar(const FString& handle, const int & len, char * arg)
{
	JHNET_CHECK(_inGameNetworkProcessor && _inGameNetworkProcessor->GetNetworkSystem());

	if (!isPlayerAuthority && _inGameNetworkProcessor->IsMaster()) {
		//JHNET_LOG(Error, "Only RecvSyncVar work on slave.");
		return;
	}
	if (isPlayerAuthority && HasAuthority()) {
		JHNET_LOG(Error, "Only RecvSyncVar work on not authority. Object : %s, Handle : %s", *GetObjectIDByString(), *handle);
		return;
	}
	if (!JHNET_CHECKAlreadyUseSyncVar(handle)) {
		JHNET_LOG(Error, "Not exist handle. Object : %s, Handle : %s", *GetObjectIDByString(), *handle);
		return;
	}

	auto syncVarTuple = GetSyncVarTuple(handle);
	if (get<1>(syncVarTuple) == nullptr) {
		JHNET_LOG(Error, "Not exist handle. Object : %s, Handle : %s", *GetObjectIDByString(), *handle);
		return;
	}
	if (get<0>(syncVarTuple) > len) {
		JHNET_LOG(Error, "Not valid argType. Object : %s, Handle : %s", *GetObjectIDByString(), *handle);
		return;
	}

	char* buf = _inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->PopObject();
	Deserialize(arg, buf, len);
	if (get<1>(syncVarTuple) == nullptr) {
		JHNET_LOG_S(Error);
		_inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
		return;
	}
	memcpy(get<1>(syncVarTuple), buf, len);
	auto synvarDelegate = get<2>(syncVarTuple);
	if (synvarDelegate != nullptr && synvarDelegate->IsBound()) {
		if (synvarDelegate->GetUObject() == nullptr) {
			JHNET_LOG_S(Error);
			_inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
			return;
		}
		synvarDelegate->Execute(len, buf);
	}
	_inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
}

void UNetworkBaseCP::SetSyncVarByInt(const FString& handle, int32 arg, bool isReliable)
{
	SetSyncVar(handle, sizeof(int32), (char*)&arg, isReliable);
}

void UNetworkBaseCP::SetSyncVarByFloat(const FString& handle, float arg, bool isReliable)
{
	SetSyncVar(handle, sizeof(float), (char*)&arg, isReliable);
}

void UNetworkBaseCP::SetSyncVarByString(const FString& handle, const FString & arg, bool isReliable)
{
	SetSyncVar(handle, arg.Len(), (char*)*arg, isReliable);
}

void UNetworkBaseCP::SetSyncVarByBool(const FString& handle, bool arg, bool isReliable)
{
	SetSyncVar(handle, 1, (char*)&arg, isReliable);
}

UNetworkBaseCP::~UNetworkBaseCP()
{
	for (auto i : _rpcFunctions) {
		if (i.Value == nullptr) continue;
		delete i.Value;
	}
	for (auto i : _hookSyncVars) {
		auto tupleVal = i.Value;
		if (get<2>(tupleVal) == nullptr) continue;
		delete get<2>(tupleVal);
	}
	_hookSyncVars.Empty();
	_rpcFunctions.Empty();
}

FRPCFunction * UNetworkBaseCP::CreateBindableDelegateFunction(const FString& idenHandle)
{
	FRPCFunction** rpcFunction = _rpcFunctions.Find(idenHandle);
	if (rpcFunction && *rpcFunction) return nullptr;
	FRPCFunction* newFunction = new FRPCFunction();
	_rpcFunctions.Add(idenHandle, newFunction);
	return newFunction;
}

void UNetworkBaseCP::SendSyncVarToServer(const FString& handle, const int & len, char * arg, bool isReliable)
{

	FC_InGame_SyncVar cInGameSyncVar;
	cInGameSyncVar.objectHandle = JHNETSerializer::FStringToBuf(GetObjectIDByString());
	cInGameSyncVar.idenHandle = JHNETSerializer::FStringToBuf(handle);
	cInGameSyncVar.argBuf.len = len;
	memcpy(cInGameSyncVar.argBuf.buf, arg, len);

	if (_inGameNetworkProcessor && _inGameNetworkProcessor->GetNetworkSystem() != nullptr) {
		_inGameNetworkProcessor->Send((FPacketStruct*)&cInGameSyncVar, isReliable);
	}
}

bool UNetworkBaseCP::JHNET_CHECKAlreadyUseSyncVar(const FString & handle)
{
	if (_hookSyncVars.Find(handle) != nullptr) return true;
	if (_syncVars.Find(handle) != nullptr) return true;
	return false;
}

tuple<int32, void*, FRPCFunction*> UNetworkBaseCP::GetSyncVarTuple(const FString& handle)
{
	auto hookResult = _hookSyncVars.Find(handle);
	if (hookResult != nullptr) {
		return *hookResult;
	}
	auto syncResult = _syncVars.Find(handle);
	if (syncResult != nullptr) {
		return make_tuple(get<0>(*syncResult), get<1>(*syncResult), nullptr);
	}
	return make_tuple(0, nullptr, nullptr);
}

bool UNetworkBaseCP::JHNET_CHECKFunction(FRPCFunction ** targetFunction)
{
	if (!targetFunction || !(*targetFunction) && !((*targetFunction)->IsBound())) {
		return false;
	}
	return true;
}

void UNetworkBaseCP::SendRPCToServer(const FString& idenHandle, const ENetRPCType& type, const int& len, char * arg, bool isReliable)
{
	JHNET_CHECK(_inGameNetworkProcessor);
	JHNET_CHECK(_inGameNetworkProcessor->GetNetworkSystem());

	FC_InGame_RPC cInGameRPC;
	cInGameRPC.rpcType = static_cast<char>(type);
	cInGameRPC.objectHandle = JHNETSerializer::FStringToBuf(GetObjectIDByString());
	cInGameRPC.idenHandle = JHNETSerializer::FStringToBuf(idenHandle);
	cInGameRPC.argBuf.len = len;
	memcpy(cInGameRPC.argBuf.buf, arg, len);

	_inGameNetworkProcessor->Send((FPacketStruct*)&cInGameRPC, isReliable);
}

void UNetworkBaseCP::ExecuteNetFunc(const FString& idenHandle, ENetRPCType type, int len, char * arg, bool isReliable, bool excuteSelf)
{
	FRPCFunction** function = _rpcFunctions.Find(idenHandle);
	if (!JHNET_CHECKFunction(function)) {
		JHNET_LOG_ERROR("Not registered function. handle = %s", *idenHandle);
		return;
	}

	// Just execute.
	switch (type)
	{
	case ENetRPCType::MULTICAST:
	{
		if(excuteSelf) ExecutedNetFunc(idenHandle, len, arg);
		SendRPCToServer(idenHandle, type, len, arg, isReliable);
		break;
	}
	case ENetRPCType::MASTER:
	{
		// If master? just run.
		if ((!_inGameNetworkProcessor || _inGameNetworkProcessor->IsMaster()) && excuteSelf) {
			ExecutedNetFunc(idenHandle, len, arg);
		}
		else {
			SendRPCToServer(idenHandle, type, len, arg, isReliable);
		}
		break;
	}
	case ENetRPCType::Max:
		break;
	default:
		JHNET_LOG(Error, "Unkown RPC type.");
		break;
	}
}

void UNetworkBaseCP::ExecutedNetFunc(const FString& idenHandle, int len, char * arg)
{
	FRPCFunction** function = _rpcFunctions.Find(idenHandle);
	if (!function || !*function) {
		JHNET_LOG(Error, "Not registered function. handle : %s", *idenHandle);
		return;
	}
	if (JHNET_CHECKFunction(function)) {
		if ((*function)->GetUObject() == nullptr) {
			JHNET_LOG_S(Error);
			return;
		}
		(*function)->Execute(len, arg);
	}
	else {
		JHNET_LOG_ERROR("Not registered function. handle = %s", *idenHandle);
	}
}

void UNetworkBaseCP::ExecuteNetFuncByInt(const FString& idenHandle, ENetRPCType type, int32 arg, bool isReliable)
{
	char* buf = GetNetworkSystem()->bufObjectPool->PopObject();
	int len = IntSerialize(buf, arg);
	ExecuteNetFunc(idenHandle, type, len, buf, isReliable);
	GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
}

void UNetworkBaseCP::ExecuteNetFuncByFloat(const FString& idenHandle, ENetRPCType type, float arg, bool isReliable)
{
	char* buf = GetNetworkSystem()->bufObjectPool->PopObject();
	int len = FloatSerialize(buf, arg);
	ExecuteNetFunc(idenHandle, type, len, buf, isReliable);
	GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
}

void UNetworkBaseCP::ExecuteNetFuncByString(const FString& idenHandle, ENetRPCType type, const FString& arg, bool isReliable)
{
	JHNET_CHECK(_inGameNetworkProcessor);
	JHNET_CHECK(_inGameNetworkProcessor->GetNetworkSystem());

	std::string siString(TCHAR_TO_UTF8(*arg));
	char* buf = _inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->PopObject();
	int len = StringSerialize(buf, siString);
	ExecuteNetFunc(idenHandle, type, len, buf, isReliable);
	_inGameNetworkProcessor->GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
}


void UNetworkBaseCP::ExecuteNetFuncByVoid(const FString& idenHandle, ENetRPCType type, bool isReliable)
{
	ExecuteNetFunc(idenHandle, type, 0, nullptr, isReliable);
}

void UNetworkBaseCP::ExecuteNetFuncByBool(const FString& handle, ENetRPCType type, bool arg, bool isReliable)
{
	char* buf = GetNetworkSystem()->bufObjectPool->PopObject();
	int len = BoolSerialize(buf, arg);
	ExecuteNetFunc(handle, type, len, buf, isReliable);
	GetNetworkSystem()->bufObjectPool->ReturnObject(buf);
}

void UNetworkBaseCP::SaveRPCHandle(int newVal, void* classAdress, char* functionName)
{
	FString handleString(functionName + FString::Printf(TEXT("%d"), classAdress));
	_rpcHandleSaver.Add(handleString, newVal);
}

int UNetworkBaseCP::GetRPCHandle(void* classAdress, char* functionName)
{
	FString handleString(functionName + FString::Printf(TEXT("%d"), classAdress));
	if(_rpcHandleSaver.Contains(handleString)) return _rpcHandleSaver[handleString];
	else {
		JHNET_LOG(Error, "%s", *(GetOwner()->GetName()));
		JHNET_LOG(Error, "Not exist handle!!");
		return 0;
	}
}