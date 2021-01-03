#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Processor/TCP/TCPReceiveProcessor.h"
#include "Processor/UDP/UDPReceiveProcessor.h"
#include "Processor/Manager/PlayerManager.h"
#include "Processor/Manager/RoomManager.h"
#include "NetworkModule/MessageQueue.h"
#include "Processor/DataType/ObjectPool.h"
#include "GameProcessorMessage.h"
#include "NetworkModule/PacketStruct.h"
#include <thread>

class CGameProcessor {
public:
	CGameProcessor() {}
	~CGameProcessor() {}

	bool Run();
	void Shutdown();

	void PostAcceptMessage(CSocket* socket, const FSocketAddrIn& clientAddr);
	void PostTCPMessage(CSocket* socket, char* buf, const int& len);
	void PostUDPMessage(const FSocketAddrIn& addr, char* buf, const int& len);
	void PostCloseMessage(CSocket* socket);

	void Send(CSocket* socket, FPacketStruct* packetStruct);
	void SendTo(const FSocketAddrIn& sockaddr, FPacketStruct* packetStruct);

	void Send(CSocket* socket, const char* buf, const int& sendLen);
	void SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen);
	void Close(CSocket* socket);

	void PrintObjectPools();
	void Print();
	void PrintSummary();

	CServerNetworkSystem* ServerNetworkSystem;
	CPlayerManager* PlayerManager;
	CRoomManager* RoomManager;
	CTCPReceiveProcessor* TCPReceiveProcessor;
	CUDPReceiveProcessor* UDPReceiveProcessor;
		
	// from -> to�� �÷��̾��� ������ �Ѱ��ش�.
	// from�� �����ǰ�, to�� ����ȴ�.
	// old�� �����ϴ� ������ �Ұ����� �Ķ���͸� �����ϰ� ��� �Ѿ��.
	CPlayer* AbsorbPlayer(CPlayer* from_new, CPlayer* to_old);

	CObjectPool<char>* bufObjectPool = nullptr;
	
private:
	// �޼��� ó�� ����
	void OnAcceptMessage(FAcceptMessage* msg);
	void OnTCPMessage(FTCPMessage* msg);
	void OnUDPMessage(FUDPMessage* msg);
	void OnCloseMessage(FCloseMessage* msg);

	void GameLogicThread();
	void Update();

	// �÷��̾� ���� ����
	void CloseConnection(CSocket* player);

	void WriteLog(const ELogLevel& level, const std::string& msg);

	bool _isRun;
	std::thread* _logicThread;
	CObjectPool<FAcceptMessage>* _acceptMsgObjectPool;
	CObjectPool<FTCPMessage>* _tcpMsgObjectPool;
	CObjectPool<FUDPMessage>* _udpMsgObjectPool;
	CObjectPool<FCloseMessage>* _closeMsgObjectPool;
	CMessageQueue<FGameProcessorMessage*>* _messageQueue;
	CMessageQueue<FGameProcessorMessage*>::QueueType _currentQueue;
};