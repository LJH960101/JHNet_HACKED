#pragma once
/*
	레플리케이션 시스템(SyncVar, RPC)에서 신뢰성을 포기하고 속도를 갖도록 하기위한 UDP 프로토콜의 수신 담당하는 클래스.
*/

#include "JHNETGameServer.h"
#include <string>
#include "NetworkModule/Log.h"
#include "Processor/DataType/Overlapped.h"
#include "Processor/DataType/Socket.h"

class CPlayer;

class CUDPReceiveProcessor
{
public:
	CUDPReceiveProcessor();
	~CUDPReceiveProcessor() {}

	// return false : ERROR
	bool ReceiveData(const FSocketAddrIn& addr, char* buf, int receiveLen);
	void SetGameProcessor(class CGameProcessor* gameProcessor);

private:
	class CGameProcessor* GameProcessor;

	class CServerNetworkSystem* ServerNetworkSystem;

	void WriteLog(ELogLevel level, std::string msg);

	// 메세지 처리부

	void HeatBeat(CPlayer*& player, char* recvBuf, int& cursor, const FSocketAddrIn& clientAddr);
	void UDPReg(CPlayer*& player, char* recvBuf, int& cursor, const FSocketAddrIn& clientAddr);
	
	// InGame
	void InGame_RPC(CPlayer*& player, char* recvBuf, int& cursor);
	void InGame_SyncVar(CPlayer*& player, char* recvBuf, int& cursor);
};