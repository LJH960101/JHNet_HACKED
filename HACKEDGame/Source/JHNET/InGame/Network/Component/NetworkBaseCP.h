// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "JHNET.h"
#include "NetworkBaseMacro.h"
#include "Components/ActorComponent.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/NetworkData.h"
#include <tuple>
#include <functional>
#include <memory>
#include "NetworkBaseCP.generated.h"


class AInGameNetworkProcessor;
class UNetworkSystem;

// RPC Function Delegate Type
DECLARE_DELEGATE_TwoParams(FRPCFunction, int32, char*);

// RPC type
UENUM(BlueprintType)
enum  class ENetRPCType : uint8
{
	MULTICAST	UMETA(DisplayName = "MULTICAST"),	// Function call to other clients and own.
	MASTER		UMETA(DisplayName = "MASTER"),		// Function call to master.
	Max

};

// 핸들러에 사용되는 바이트의 최대수
const int MAX_HANDLE_BYTE = 50;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JHNET_API UNetworkBaseCP : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	bool OnSync(){ return _bOnSync; }

	UFUNCTION(BlueprintCallable)
	void EnableSync(bool isOn);
	
	UFUNCTION(BlueprintPure)
	bool IsEditor();

	UFUNCTION(BlueprintPure)
	bool IsSingle();

	UFUNCTION(BlueprintPure)
	int GetPlayerNums();

	// Sync Stop
	void PlayStopSyncTime(float time);
	FTimerHandle SyncStopTimerHandle;

	// Network Identify ****
	UFUNCTION(BlueprintPure, Category = "Network")
	bool HasAuthority();
	UFUNCTION(BlueprintPure, Category = "Network")
	bool IsMaster();
	UFUNCTION(BlueprintPure, Category = "Network")
	bool IsSlave();
	UFUNCTION(BlueprintPure, Category = "Network")
	bool OnServer();
	UFUNCTION(BlueprintPure, Category = "Network")
	bool IsPlayerAuthority();
	UFUNCTION(BlueprintPure, Category = "Network")
	FString GetObjectIDByString();
	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetAuthority(int32 slot);
	UFUNCTION(BlueprintCallable, Category = "Network")
	void NetworkDestroy();
	FString _objectID;
	UFUNCTION(BlueprintPure, Category = "Network")
	int32 GetCurrentSlot();
	UFUNCTION(BlueprintPure, Category = "Network")
	int32 GetCurrentAuthority();

	UFUNCTION(BlueprintPure, Category = "Network")
	AInGameNetworkProcessor* GetInGameNetworkProcessor();

	UFUNCTION(BlueprintPure, Category = "Network")
	UNetworkSystem* GetNetworkSystem();

	// Set Object ID In InGameManager
	UFUNCTION()
	void SetObjectIDByNetworkID(const FString& id);

	// Network Sync Var ****
	bool JHNET_CHECKAlreadyUseSyncVar(const FString& handle);

	// Only SyncVar works on primitive types.(int, char, float... not string vector)
	bool BindSyncVar(const FString& handle, int32 len, void * adress);
	FRPCFunction* CreateBindableSyncVarDelegate(const FString& handle, int32 len, void* adress);
	bool SetSyncVar(const FString& handle, int len, char* arg, bool isReliable = true);
	void RecvSyncVar(const FString& handle, const int& len, char* arg);
	// BP Function Calls
	UFUNCTION(BlueprintCallable)
	void SetSyncVarByInt(const FString& handle, int32 arg, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void SetSyncVarByFloat(const FString& handle, float arg, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void SetSyncVarByString(const FString& handle, const FString& arg, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void SetSyncVarByBool(const FString& handle, bool arg, bool isReliable = true);

	// Network RPC ****
	FRPCFunction* CreateBindableDelegateFunction(const FString& handle);
	// Executed request function
	void ExecuteNetFunc(const FString& handle, ENetRPCType type, int len, char* arg, bool isReliable = true, bool excuteSelf = true);
	// Executed answer or own function
	void ExecutedNetFunc(const FString& handle, int len, char* arg);
	// BP Function Calls
	UFUNCTION(BlueprintCallable)
	void ExecuteNetFuncByInt(const FString& handle, ENetRPCType type, int32 arg, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void ExecuteNetFuncByFloat(const FString& handle, ENetRPCType type, float arg, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void ExecuteNetFuncByString(const FString& handle, ENetRPCType type, const FString& arg, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void ExecuteNetFuncByVoid(const FString& handle, ENetRPCType type, bool isReliable = true);
	UFUNCTION(BlueprintCallable)
	void ExecuteNetFuncByBool(const FString& handle, ENetRPCType type, bool arg, bool isReliable = true);
	int GetRPCHandle(void* classAdress, char* functionName);

public:	
	// Sets default values for this component's properties
	UNetworkBaseCP();
	~UNetworkBaseCP();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool _bOnSync;
	void _OnStopSyncEnd();

	UPROPERTY()
		AInGameNetworkProcessor* _inGameNetworkProcessor;

	// Network Identify ****
	bool isPlayerAuthority = false;
	UFUNCTION()
	void SetObjectIDByAuto();
	bool _isAuto = true;
	bool _isRegistered = false;
	int32 _authorityId = 0;

	// Network Sync Var ****
	// Only sync Value
	TMap<FString, std::tuple<int32, void*>> _syncVars;
	// Sync value and call hook function
	TMap<FString, std::tuple<int32, void*, FRPCFunction*>> _hookSyncVars;
	void SendSyncVarToServer(const FString& handle, const int& len, char * arg, bool isReliable);
	std::tuple<int32, void*, FRPCFunction*> GetSyncVarTuple(const FString& handle);

	// Network RPC ****
	TMap<FString, FRPCFunction*> _rpcFunctions;
	bool JHNET_CHECKFunction(FRPCFunction** targetFunction);
	// create buf and send to server.
	void SendRPCToServer(const FString& handle, const ENetRPCType& type, const int& len, char * arg, bool isReliable);
	int _rpcHandleCounter = 1000;
	void SaveRPCHandle(int newVal, void* classAdress, char* functionName);
	UPROPERTY()
	TMap<FString, int> _rpcHandleSaver;

	// Already declared RPC
	RPC_FUNCTION(UNetworkBaseCP, __RPCSetAuthority, int)
	void __RPCSetAuthority(int slot);

	RPC_FUNCTION(UNetworkBaseCP, __RPCNetworkDestroy)
	void __RPCNetworkDestroy();
};
