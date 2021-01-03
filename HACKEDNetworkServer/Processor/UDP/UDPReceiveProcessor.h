#pragma once
/*
	���ø����̼� �ý���(SyncVar, RPC)���� �ŷڼ��� �����ϰ� �ӵ��� ������ �ϱ����� UDP ���������� ���� ����ϴ� Ŭ����.
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

	// �޼��� ó����

	void HeatBeat(CPlayer*& player, char* recvBuf, int& cursor, const FSocketAddrIn& clientAddr);
	void UDPReg(CPlayer*& player, char* recvBuf, int& cursor, const FSocketAddrIn& clientAddr);
	
	// InGame
	void InGame_RPC(CPlayer*& player, char* recvBuf, int& cursor);
	void InGame_SyncVar(CPlayer*& player, char* recvBuf, int& cursor);
};