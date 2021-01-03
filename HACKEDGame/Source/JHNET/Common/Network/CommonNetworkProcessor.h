// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#define DEBUG_RECV
#include "JHNET.h"
#include "GameFramework/Actor.h"
#include "NetworkModule/PacketStruct.h"
#include "Common/Widget/WGFade.h"
#include "CommonNetworkProcessor.generated.h"

struct FReciveData;
class UJHNETGameInstance;
class UNetworkSystem;

typedef TTuple<char*, int> TPacket;
UCLASS()
class JHNET_API ACommonNetworkProcessor : public AActor
{
	GENERATED_BODY()
	
public:
	void DisableRecv(bool isOn);

	// When true, Network connection will be cancle automatically.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWithoutNetwork = false;

	// Sets default values for this actor's properties
	ACommonNetworkProcessor();
	virtual void PostInitializeComponents() override;

	// SEND
	void Send(FPacketStruct* packetStruct, bool isTCP = true);

	virtual void Tick(float DeltaSeconds) override;

	// 패킷을 모아서 보낸다.
	void SendProc(float DeltaSeconds);

	// 연결 상태를 파악한다.
	void GetServerState(float DeltaSeconds);
	bool _lastIsOn = false;

protected:
	const int fadeZIndex = 100;
	UPROPERTY()
	TSubclassOf<class UWGFade> fadeScreenClass;
	UWGFade* fadeScreen;

	// Called when the game starts or when spawned
	virtual void RecvProc(FReciveData& data);
	UJHNETGameInstance* GetGameInstance();
	UNetworkSystem* GetNetworkSystem();

	// Called when room is changed.
	virtual void OnRefreshRoom();

	// 서버에 접속되었을때 호출됩니다.
	virtual void OnConnected();

	virtual void BeginPlay() override;

private:
	bool _bOnDisableRecv = false;

	UJHNETGameInstance* _gameInstance;
	class UNetworkSystem* _netSystem;

	// 배열 0은 TCP, 1은 UDP로 사용한다.
	TQueue<TPacket>  _savedPackets[2];

	UPROPERTY()
	TSubclassOf<class UUserWidget> _serverConnectWGClass;
	UPROPERTY()
	class UUserWidget* _serverConnectWG = nullptr;

	// 메세지 처리 함수

	// Common
	void Common_Echo(FReciveData& data, int& cursor);
	void Common_Ping(FReciveData& data, int& cursor);
	void Common_RequestID(FReciveData& data, int& cursor);

	void Room_RoomInfo(FReciveData& data, int& cursor);
	void Reconnect(FReciveData& data, int& cursor);
	void UDP_UDPReconnectRequest(FReciveData& data, int& cursor);
	void UDP_TCPReconnectRequest(FReciveData& data, int& cursor);
};
