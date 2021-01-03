/*
	Main Network System
*/

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "JHNET.h"
#include "NetworkModule/NetworkInfo.h"
#include "AllowWindowsPlatformTypes.h"
#include <WinSock2.h>
#include "HideWindowsPlatformTypes.h"
#include <mutex>
#include <thread>
#include "Containers/Queue.h"
#include "ReciveDataPool.h"
#include "NetworkModule/ObjectPool.h"
#include "NetworkModule/PacketStruct.h"
#include "NetworkSystem.generated.h"

class AInGameManager;

enum ESocketInitState : uint8 {
	INIT_START, // �ʱ�ȭ���� ����
	INIT_SUCCESS, // �ʱ�ȭ ����
	INIT_FAILED // �ʱ�ȭ ����(�ʱ�ȭ�� ������) �Ǵ� ���� ����
};

UCLASS(Config=HackedNetwork)
class JHNET_API UNetworkSystem : public UObject
{
	friend class ACommonNetworkProcessor;

	GENERATED_BODY()
public:
	UNetworkSystem();
	~UNetworkSystem();

	CObjectPool<char>* bufObjectPool;

	UFUNCTION(BlueprintCallable)
	bool OnSteam();

	UFUNCTION(BlueprintCallable)
	bool OnServer();

	void GetData(FReciveData*& outData);

	void Send(const char* buf, int len, bool isTCP, bool withoutLen = false);
	void Send(FPacketStruct* packetStruct, bool isTCP, bool withoutLen = false);

	void ReturnObject(FReciveData* object);

	UINT64 GetSteamID();

	UFUNCTION(BlueprintCallable)
	FString GetSteamIDofString();

	UFUNCTION(BlueprintCallable)
	void SetOnDebugReset();

	bool GetReconnectMode();
	void SetReconnectMode(bool isOn);

	UFUNCTION(BlueprintCallable)
	void Init();
	UFUNCTION(BlueprintCallable)
	void Shutdown();

	void SetSteamID_DUBGE(const UINT64& steamID);

	// Steam
	UTexture2D* GetSteamAvartar(const UINT64 steamID);
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSteamAvartar(const FString steamID);
	FString GetSteamName(const UINT64 steamID);
	UFUNCTION(BlueprintCallable)
	FString GetSteamName(const FString steamID);
	UFUNCTION(BlueprintCallable)
	FString GetMySteamName();

	void SetSteamID(int32 slot, UINT64 steamID);
	UINT64 GetSteamID(int32 slot);

	// InGameManager
	void SetInGameManager(AInGameManager* inGameManager);

	UFUNCTION(BlueprintPure, Category = "Network")
	AInGameManager* GetInGameManager();
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentSlot();
	bool ConnectUDP();

	bool ReconnectUDP();

	// Automatically true on reconnect.
	bool bNeedToResync = false;

	// ���� ����

	// �񵿱� ���� ����
	void PostInitSocket(bool isReconnect = false);
	ESocketInitState GetSocketState();

	// ���� ������ �����Ѵ�.
	UFUNCTION(BlueprintCallable, Category = "Socket")
	void StopToInit();

private:
	bool _bOnReconnect = false;
	bool _bOnDebugReset = false;

	UPROPERTY()
	UReciveDataPool* _dataPool;

	// Steam
	bool InitSteam();
	void ShutdownSteam();
	bool bOnSteam;
	UINT64 defaultSteamID = 0;
	UINT64 steamIds[MAX_PLAYER];

	UPROPERTY(Config)
	FString ipAdress;

	// Socket

	// isReconnect�� ������ �����Ǿ� �������ϴ��� ���θ� ��Ÿ���ϴ�.
	void InitSocket(bool isReconnect = false);
	bool InitTCP(bool isReconnect = false);
	bool InitUDP();

	// ������ ������ �õ��ϴ� ������
	std::thread* socketInitThread;

	// ���� ���� ����
	ESocketInitState currentInitState;

	std::mutex shutdownLocker;
	void ShutdownSocket();
	int RecvUDP(char* buf, int size = PACKET_BUFSIZE);
	bool bOnServer;

	// UDP
	SOCKET udpSock = NULL;
	SOCKADDR_IN udpAddr;
	void UDPRecvThread();
	// Used by TCPRecvThread only buffer!! Do not use in other function.
	char udpRecvBuf[PACKET_BUFSIZE];
	std::thread* hUdpThread = nullptr;

	// TCP
	SOCKET tcpSock = NULL;
	void TCPRecvThread();
	// Used by TCPRecvThread only buffer!! Do not use in other function.
	char tcpRecvBuf[PACKET_BUFSIZE];
	std::thread* hTcpThread = nullptr;

	// Data saver
	std::recursive_mutex recvMutex;
	TQueue<FReciveData*> recvQueue;

	// Game System
	int32 _currentSlot = 0;

	// Managers
	AInGameManager* InGameManager;
	void SetCurrentSlot(int32 slot);

	void AddSavedData(FReciveData*& newData, FReciveData*& savedData, const char* recvBuf, const int& retval);
	void DelayData(FReciveData*& newData, FReciveData*& savedData, const int& delayIndex);
};