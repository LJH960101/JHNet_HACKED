// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonNetworkProcessor.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/NetworkInfo.h"
#include "Blueprint/UserWidget.h"
#include "InGame/JHNETTool.h"

using namespace JHNETSerializer;

void ACommonNetworkProcessor::DisableRecv(bool isOn)
{
	_bOnDisableRecv = isOn;
}

// Sets default values
ACommonNetworkProcessor::ACommonNetworkProcessor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetClassAsset(fadeScreenClass, UWGFade, "WidgetBlueprint'/Game/NetworkUI/WB_Fade.WB_Fade_C'");

	static ConstructorHelpers::FClassFinder<UUserWidget> ServerConnectWidget(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_Connect.WB_Connect_C'"));
	if (ServerConnectWidget.Succeeded()) {
		_serverConnectWGClass = ServerConnectWidget.Class;
	}
}

UJHNETGameInstance* ACommonNetworkProcessor::GetGameInstance() {
	return _gameInstance;
}

UNetworkSystem* ACommonNetworkProcessor::GetNetworkSystem()
{
	return _netSystem;
}

// Called when the game starts or when spawned
void ACommonNetworkProcessor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	_gameInstance = Cast<UJHNETGameInstance>(AActor::GetGameInstance());
	JHNET_CHECK(_gameInstance);
	_netSystem = _gameInstance->GetNetworkSystem();
	JHNET_CHECK(_netSystem);

	if (_serverConnectWGClass) {
		if (!_serverConnectWG) _serverConnectWG = CreateWidget<UUserWidget>(GetWorld(), _serverConnectWGClass);
		if (_serverConnectWG && !_serverConnectWG->IsInViewport()) _serverConnectWG->AddToViewport(5);
	}
}

void ACommonNetworkProcessor::Send(FPacketStruct* packetStruct, bool isTCP /*= true*/)
{
	// 연결이 안되어있을 경우에는 보내지 않는다.
	JHNET_CHECK(_gameInstance);
	if (!_gameInstance->GetNetworkSystem()->OnServer()) return;

	char* buf = _gameInstance->GetNetworkSystem()->bufObjectPool->PopObject();
	packetStruct->Serialize(buf);
	if (isTCP) {
		_savedPackets[0].Enqueue(TPacket(buf, (int)packetStruct->GetSize()));
	}
	else {
		_savedPackets[1].Enqueue(TPacket(buf, (int)packetStruct->GetSize()));
	}
}

void ACommonNetworkProcessor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	JHNET_CHECK(_gameInstance);
	if(_gameInstance->GetNetworkSystem()->OnServer()) SendProc(DeltaSeconds);
	GetServerState(DeltaSeconds);

	if (_bOnDisableRecv) return;
	while (true)
	{
		FReciveData* newData;
		_gameInstance->GetNetworkSystem()->GetData(newData);
		if (newData == nullptr) break;
		RecvProc(*newData);
		_gameInstance->GetNetworkSystem()->ReturnObject(newData);
	}
}

void ACommonNetworkProcessor::OnConnected()
{

}

void ACommonNetworkProcessor::BeginPlay()
{
	Super::BeginPlay();

	if (!fadeScreen) fadeScreen = CreateWidget<UWGFade>(GetWorld(), fadeScreenClass);
	if (!fadeScreen->IsInViewport()) fadeScreen->AddToViewport(fadeZIndex);
	if (fadeScreen) fadeScreen->FadeIn(1.0f);

	if (bWithoutNetwork) _gameInstance->GetNetworkSystem()->StopToInit();
	_netSystem = _gameInstance->GetNetworkSystem();
}

void ACommonNetworkProcessor::SendProc(float DeltaSeconds)
{
	JHNET_CHECK(_gameInstance);

	char* pSendBuf = _gameInstance->GetNetworkSystem()->bufObjectPool->PopObject();

	// i = 0 -> TCP
	// i = 1 -> UDP
	for (int i = 0; i <= 1; ++i) {
		// 1틱 동안 쌓인 패킷을 합쳐서 보낸다.
		TQueue<TPacket> _tempQueue;

		int allLen = 0; // 총 len 수, MAX_BUF를 넘어선 안된다.
		int packetCount = 0;
		while (!_savedPackets[i].IsEmpty()) {
			TPacket packet;
			_savedPackets[i].Peek(packet);
			// 1회 버퍼 한도를 넘었다면 더이상 담지 않는다.
			if (allLen + packet.Get<1>() +
				((packetCount + 5) * sizeof(int))
				>= PACKET_BUFSIZE) {
				JHNET_LOG_WARNING("Buffer limit over!! %d", allLen + packet.Get<1>() +
					((packetCount + 5) * sizeof(int)));
				break;
			}
			else {
				// 넘지 않았다면 저장.
				allLen += packet.Get<1>();
				++packetCount;
				_savedPackets[i].Pop();
				_tempQueue.Enqueue(packet);
			}
		}

		// 저장한 큐의 내용을 모두 합친다.
		int cursor = 0;
		while (!_tempQueue.IsEmpty()) {
			TPacket packet;
			_tempQueue.Dequeue(packet);

			// 큐의 내용에 패킷 크기를 합쳐서 최종 버퍼에 합친다.
			cursor += IntSerialize(pSendBuf + cursor, packet.Get<1>());
			memcpy(pSendBuf + cursor, packet.Get<0>(), packet.Get<1>());
			cursor += packet.Get<1>();
			_gameInstance->GetNetworkSystem()->bufObjectPool->ReturnObject(packet.Get<0>());
			if (cursor >= PACKET_BUFSIZE ||
				cursor > allLen + sizeof(int) * packetCount) {
				JHNET_LOG(Error, "Critical Error : Not matched packet...");
				_gameInstance->GetNetworkSystem()->bufObjectPool->ReturnObject(pSendBuf);
				return;
			}
		}

		// 합친 버퍼를 전송한다.
		if (cursor > 0) {
			if (i == 0) _gameInstance->GetNetworkSystem()->Send(pSendBuf, cursor, true, true);
			else _gameInstance->GetNetworkSystem()->Send(pSendBuf, cursor, false, true);
		}
	}
	_gameInstance->GetNetworkSystem()->bufObjectPool->ReturnObject(pSendBuf);
}

void ACommonNetworkProcessor::GetServerState(float DeltaSeconds)
{
	JHNET_CHECK(_gameInstance);

	if (_netSystem) {
		ESocketInitState state = _netSystem->GetSocketState();
		switch (state) {
		case INIT_START:
			if (_serverConnectWG && !_serverConnectWG->IsInViewport()) _serverConnectWG->AddToViewport(5);
			_lastIsOn = false;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);
			break;
		case INIT_SUCCESS:
			if (_serverConnectWG && _serverConnectWG->IsInViewport()) _serverConnectWG->RemoveFromParent();
			if (!_lastIsOn) {
				OnConnected();
				_lastIsOn = true;
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
			}
			break;
		case INIT_FAILED:
			if (_serverConnectWG && _serverConnectWG->IsInViewport()) _serverConnectWG->RemoveFromParent();
			_lastIsOn = false;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
			break;
		default:
			JHNET_LOG_ERROR("Unknown state.");
			break;
		}
	}
}

void ACommonNetworkProcessor::RecvProc(FReciveData& data)
{
	JHNET_CHECK(_gameInstance);

	int cursor = 0, currentCursor;
	while (cursor < data.len) {
		JHNET_CHECK(cursor >= 0 && cursor <= PACKET_BUFSIZE);
		int bufLen = IntDeserialize(data.buf, &cursor);
		if (!(bufLen >= 0 && bufLen <= PACKET_BUFSIZE)) {
			JHNET_LOG_ERROR("ASSERTION : 'bufLen >= 0 && bufLen <= PACKET_BUFSIZE' buflen = %d, cursor = %d", bufLen, cursor)
			return;
		}

		currentCursor = cursor;
		FPacketStruct newPacket;
		newPacket.Deserialize(data.buf, &currentCursor);

		switch (newPacket.type)
		{
		case EMessageType::Common_Echo:
		{
			Common_Echo(data, cursor);
			break;
		}
		case EMessageType::Common_Ping:
		{
			Common_Ping(data, cursor);
			break;
		}
		case EMessageType::S_Common_RequestId:
		{
			Common_RequestID(data, cursor);
			break;
		}
		case EMessageType::S_Room_Info:
		{
			Room_RoomInfo(data, cursor);
			break;
		}
		case EMessageType::S_Reconnect:
		{
			Reconnect(data, cursor);
			break;
		}
		case EMessageType::S_UDP_UDPReconnectRequest:
		{
			UDP_UDPReconnectRequest(data, cursor);
			break;
		}
		case EMessageType::S_UDP_TCPReconnectRequest:
		{
			UDP_TCPReconnectRequest(data, cursor);
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

void ACommonNetworkProcessor::OnRefreshRoom()
{

}

void ACommonNetworkProcessor::Common_Echo(FReciveData& data, int& cursor)
{
	F_Common_Echo commonEcho;
	commonEcho.Deserialize(data.buf, &cursor);

	JHNET_LOG(Warning, "%s", commonEcho.newString.buf);
}

void ACommonNetworkProcessor::Common_Ping(FReciveData& data, int& cursor)
{
	F_Common_Ping commonPing;
	commonPing.Deserialize(data.buf, &cursor);

	Send((FPacketStruct*)& commonPing, true);
}

void ACommonNetworkProcessor::Common_RequestID(FReciveData& data, int& cursor)
{
	FS_Common_RequestID sCommonRequestID;
	sCommonRequestID.Deserialize(data.buf, &cursor);

	UINT64 steamID = _gameInstance->GetNetworkSystem()->GetSteamID();
	if (steamID == 0) return;

	FC_Common_AnswerID cCommonAnswerID;
	cCommonAnswerID.steamID = steamID;

	Send((FPacketStruct*)&cCommonAnswerID, true);
}

void ACommonNetworkProcessor::Room_RoomInfo(FReciveData& data, int& cursor)
{
	FS_Room_Info sRoomInfo;
	sRoomInfo.Deserialize(data.buf, &cursor);

	// Get 4 UINT64, find my slot and registered.
	UINT64 slot;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		slot = sRoomInfo.slot[i];
		_netSystem->SetSteamID(i, slot);

		// 나의 슬롯이라면 SetCurrentSlot 호출.
		if (slot == _netSystem->GetSteamID())
			_netSystem->SetCurrentSlot(i);
	}
	OnRefreshRoom();
}

void ACommonNetworkProcessor::Reconnect(FReciveData& data, int& cursor)
{
	FS_Reconnect sReconnect;
	sReconnect.Deserialize(data.buf, &cursor);

	_netSystem->bNeedToResync = true;
	UINT64 slot;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		slot = sReconnect.slot[i];
		if (slot == _netSystem->GetSteamID())
		{
			_netSystem->SetCurrentSlot(i);
		}
	}
	OnRefreshRoom();
}

void ACommonNetworkProcessor::UDP_UDPReconnectRequest(FReciveData& data, int& cursor)
{
	FS_UDP_UDPReconnectRequest sUDPUDPReconnectRequest;
	sUDPUDPReconnectRequest.Deserialize(data.buf, &cursor);

	JHNET_LOG_WARNING("InitUDP : %d", _netSystem->ReconnectUDP());
}

void ACommonNetworkProcessor::UDP_TCPReconnectRequest(FReciveData& data, int& cursor)
{
	FS_UDP_TCPReconnectRequest sUDPTCPReconnectRequest;
	sUDPTCPReconnectRequest.Deserialize(data.buf, &cursor);

	UINT64 slots[MAX_PLAYER];
	for (int i = 0; i < MAX_PLAYER; ++i) {
		slots[i] = _netSystem->GetSteamID(i);
	}
	_netSystem->Shutdown();
	for (int i = 0; i < MAX_PLAYER; ++i) {
		_netSystem->SetSteamID(i, slots[i]);
	}
	Sleep(1000);
	_netSystem->Init();
}
