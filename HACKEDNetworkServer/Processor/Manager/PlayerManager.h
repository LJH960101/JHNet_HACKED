#pragma once
/*
	플레이어의 정보를 관할하는 클래스
*/

#include "JHNETGameServer.h"
#include "NetworkModule/NetworkData.h"
#include "Processor/DataType/Player.h"
#include "Processor/DataType/ObjectPool.h"
#include <vector>
#include <iostream>

using std::vector;
struct FOverlapped;

class CPlayerManager
{
public:
	CPlayerManager();
	~CPlayerManager();

	// nullptr when player not exist
	CPlayer* GetPlayerById(const UINT64& steamID);
	CPlayer* GetNoUDPPlayerById(const UINT64& steamID);
	CPlayer* GetPlayerBySocket(CSocket* targetSocket);
	CPlayer* GetPlayerBySocket(SOCKET targetSocket);
	CPlayer* GetPlayerByNum(const int& num);
	CPlayer* GetPlayerByUDPAddr(const FSocketAddrIn& addr);
	CPlayer* GetPlayerByTCPAddr(const FSocketAddrIn& addr);
	CPlayer* GetNoUDPAddrPlayerByTCPAddr(const FSocketAddrIn& addr);
	bool IsContain(CPlayer* player);

	CPlayer* CreatePlayer();
	void AddPlayer(CPlayer* newPlayerInfo);
	CPlayer* EditPlayerIDBySocket(CSocket* targetSocket, const UINT64& steamID);
	int GetPlayerCount();
	void RemovePlayerById(const UINT64& playerID);
	void RemovePlayerBySocket(CSocket* playerSocket);
	void RemovePlayer(CPlayer* player);
	void AbsorbPlayer(CPlayer* from_new, CPlayer* to_old);

	void Print();
	void PrintObjectPools();
	void PrintSummary();

	void SetGameProcessor(class CGameProcessor* gameProcessor);

private:
	void ReturnPlayer(CPlayer* player);

	class CGameProcessor* GameProcessor;

	vector<CPlayer*> players;
	CObjectPool<CPlayer>* playerObjectPool;
};

