#pragma once
#include "JHNETGameServer.h"
#include "Processor/DataType/GameState.h"
#include "Processor/DataType/Socket.h"
#include <ctime>
#include <chrono>
#include <ctime>
#include "Overlapped.h"

typedef std::chrono::time_point<std::chrono::system_clock> TIME;
struct FTCPRecvOverlapped;
struct FUDPRecvOverlapped;
struct FBuffuerableOverlapped;

class CPlayer
{
public:
	UINT64 steamID;
	CSocket* socket = nullptr;
	FSocketAddrIn addr;
	FSocketAddrIn udpAddr;
	TIME lastPingTime;
	TIME lastPongTime;
	EGameState state;

	// ������ ������ �������
	char* delayedTCPData = nullptr;
	char* delayedUDPData = nullptr;
	// ������ ������ ũ��
	int delayedTCPDataLen = 0;
	int delayedUDPDataLen = 0;
	CPlayer();
	~CPlayer();

	void Init();

	// ���� ���۰� �ִ��� Ȯ���ϰ� �ִٸ� ������ ���� ���ۿ� �����ش�.
	// ���ϰ��� ���� �þ ����.
	int PopDelayData(char* buf, int& ref_len, bool isTCP);

	// ���� �����͸� delayedData�� �־��ش�.
	void PushDelayData(char* buf, const int& cursor, const int& recvLen, bool isTCP);

	// UDP Addr�� �����Ѵ�.
	// �������� �ʴ� �÷��̾��ϰ�� false ����.
	bool SetUDPAddr(const FSocketAddrIn& newAddr);

	bool IsHaveUDP() { return bHaveUDP; }
	void RemoveUDPAddr();

	void SetGameProcessor(class CGameProcessor* gameProcessor);

private:
	class CGameProcessor* GameProcessor;

	bool bHaveUDP = false;
};