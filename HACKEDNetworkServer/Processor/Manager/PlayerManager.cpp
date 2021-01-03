#include "PlayerManager.h"
#include "NetworkModule/Log.h"
#include "Server/ServerNetworkSystem.h"
#include <algorithm>
#include "Processor/Manager/RoomManager.h"
#include "Processor/DataType/Room.h"
#include "Server/IOCPServer.h"
#include "Processor/GameProcessor.h"

using namespace std;

CPlayerManager::CPlayerManager() {
	playerObjectPool = new CObjectPool<CPlayer>();
	playerObjectPool->SetWithMutex(false);
}

CPlayer* CPlayerManager::GetPlayerById(const UINT64& steamID)
{
	CPlayer* retval = nullptr;
	
	for (auto i : players) {
		if (i->steamID == steamID) {
			retval = i;
			break;
		}
	}
	return retval;
}

CPlayer* CPlayerManager::GetNoUDPPlayerById(const UINT64& steamID)
{
	CPlayer* retval = nullptr;

	for (auto i : players) {
		if (i->steamID == steamID && !(i->IsHaveUDP())) {
			retval = i;
			break;
		}
	}
	return retval;
}

CPlayer* CPlayerManager::GetPlayerBySocket(CSocket* targetSocket)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (i->socket == targetSocket) {
			retval = i;
			break;
		}
	}
	return retval;
}

CPlayer* CPlayerManager::GetPlayerBySocket(SOCKET targetSocket)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (i->socket->GetSocket() == targetSocket) {
			retval = i;
			break;
		}
	}
	return retval;
}

CPlayer* CPlayerManager::GetPlayerByNum(const int& num)
{
	CPlayer* retval = nullptr;
	if (num < (int)players.size()) {
		retval = players[num];
	}
	return retval;
}

CPlayer* CPlayerManager::GetPlayerByUDPAddr(const FSocketAddrIn & addr)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (i->IsHaveUDP() &&
			i->udpAddr.sin_addr.s_addr == addr.sin_addr.s_addr &&
			i->udpAddr.sin_port == addr.sin_port) {
			retval = i;
			break;
		}
	}
	return retval;
}

CPlayer* CPlayerManager::GetNoUDPAddrPlayerByTCPAddr(const FSocketAddrIn& addr)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (!(i->IsHaveUDP()) && i->addr.sin_addr.s_addr == addr.sin_addr.s_addr) {
			retval = i;
			break;
		}
	}
	return retval;
}

bool CPlayerManager::IsContain(CPlayer* player)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (i == player) {
			retval = i;
			break;
		}
	}
	if (retval) return true;
	else return false;
}

CPlayer* CPlayerManager::CreatePlayer()
{
	CPlayer* newPlayer = playerObjectPool->PopObject();
	newPlayer->SetGameProcessor(GameProcessor);
	newPlayer->Init();
	AddPlayer(newPlayer);
	return newPlayer;
}

CPlayer* CPlayerManager::GetPlayerByTCPAddr(const FSocketAddrIn & addr)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (i->addr.sin_addr.s_addr == addr.sin_addr.s_addr) {
			retval = i;
			break;
		}
	}
	return retval;
}

void CPlayerManager::AddPlayer(CPlayer* newPlayer)
{	
	players.push_back(newPlayer);
}

CPlayer* CPlayerManager::EditPlayerIDBySocket(CSocket* targetSock, const UINT64& steamID)
{
	CPlayer* retval = nullptr;
	for (auto i : players) {
		if (i->socket == targetSock) {
			retval = i;
			retval->steamID = steamID;
			break;
		}
	}
	return retval;
}

int CPlayerManager::GetPlayerCount()
{
	return static_cast<int>(players.size());
}

void CPlayerManager::RemovePlayerById(const UINT64& playerID)
{
	CPlayer* removeNeedPoint;
	auto i = std::begin(players);

	while (i != std::end(players)) {
		// Do some stuff
		if ((*i)->steamID == playerID)
		{
			removeNeedPoint = *i;
			players.erase(i);
			ReturnPlayer(removeNeedPoint);
			break;
		}
		else
			++i;
	}
}

void CPlayerManager::RemovePlayerBySocket(CSocket* playerSocket)
{
	CPlayer* removeNeedPoint = nullptr;
	auto i = std::begin(players);

	while (i != std::end(players)) {
		// Do some stuff
		if ((*i)->socket == playerSocket)
		{
			removeNeedPoint = *i;
			players.erase(i);
			ReturnPlayer(removeNeedPoint);
			break;
		}
		else
			++i;
	}
}

void CPlayerManager::RemovePlayer(CPlayer* player)
{
	CPlayer* removeNeedPoint;
	
	auto i = std::begin(players);

	while (i != std::end(players)) {
		// Do some stuff
		if (*i == player)
		{
			removeNeedPoint = *i;
			players.erase(i);
			ReturnPlayer(removeNeedPoint);
			break;
		}
		else
			++i;
	}
}

void CPlayerManager::AbsorbPlayer(CPlayer* from_new, CPlayer* to_old)
{
	if (!from_new || !to_old || from_new == to_old) return;

	CRoom* room = GameProcessor->RoomManager->GetGameRoom(to_old);

	if (room) to_old->state = EGameState::GAME;
	else to_old->state = EGameState::LOBBY;
	to_old->addr = from_new->addr;
	to_old->socket = from_new->socket;
	to_old->udpAddr = from_new->udpAddr;
	to_old->lastPingTime = from_new->lastPingTime;
	to_old->lastPongTime = from_new->lastPongTime;
	from_new->socket = nullptr;

	// 흡수전 플레이어를 삭제시킨다.
	RemovePlayer(from_new);
	if (!GameProcessor->PlayerManager->GetPlayerById(to_old->steamID)) {
		GameProcessor->PlayerManager->AddPlayer(to_old);
	}
}

void CPlayerManager::Print()
{
	int count = 0;
	printf("Player : \n");
	for (auto i : players) {
		printf("%d: %llu(%p)  udp? : %d\n", count++, i->steamID, i, i->IsHaveUDP());
	}
}

void CPlayerManager::PrintObjectPools()
{
	printf("CPlayerManager::PrintObjectPools\n");
	playerObjectPool->Print();
}

void CPlayerManager::PrintSummary()
{
	std::cout << "P" << GetPlayerCount() << " ";
}

void CPlayerManager::SetGameProcessor(class CGameProcessor* gameProcessor)
{
	GameProcessor = gameProcessor;
}

void CPlayerManager::ReturnPlayer(CPlayer* player)
{
	if (!player) return;
	if (player->delayedTCPData) {
		GameProcessor->bufObjectPool->ReturnObject(player->delayedTCPData);
		player->delayedTCPData = nullptr;
	}
	if (player->delayedUDPData) {
		GameProcessor->bufObjectPool->ReturnObject(player->delayedUDPData);
		player->delayedUDPData = nullptr;
	}
	playerObjectPool->ReturnObject(player);
}

CPlayerManager::~CPlayerManager()
{
	players.clear();
	if (playerObjectPool) delete playerObjectPool;
}
