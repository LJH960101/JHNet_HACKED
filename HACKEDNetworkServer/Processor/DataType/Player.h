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

	// 누락된 데이터 저장버퍼
	char* delayedTCPData = nullptr;
	char* delayedUDPData = nullptr;
	// 누락된 데이터 크기
	int delayedTCPDataLen = 0;
	int delayedUDPDataLen = 0;
	CPlayer();
	~CPlayer();

	void Init();

	// 남은 버퍼가 있는지 확인하고 있다면 꺼내서 현재 버퍼에 합쳐준다.
	// 리턴값은 새로 늘어난 길이.
	int PopDelayData(char* buf, int& ref_len, bool isTCP);

	// 남은 데이터를 delayedData에 넣어준다.
	void PushDelayData(char* buf, const int& cursor, const int& recvLen, bool isTCP);

	// UDP Addr을 설정한다.
	// 존재하지 않는 플레이어일경우 false 리턴.
	bool SetUDPAddr(const FSocketAddrIn& newAddr);

	bool IsHaveUDP() { return bHaveUDP; }
	void RemoveUDPAddr();

	void SetGameProcessor(class CGameProcessor* gameProcessor);

private:
	class CGameProcessor* GameProcessor;

	bool bHaveUDP = false;
};