#include "Room.h"
#include "Server/ServerNetworkSystem.h"
#include "Processor/UDP/UDPReceiveProcessor.h"
#include "Processor/Manager/PlayerManager.h"
#include "Processor/Manager/RoomManager.h"
#include "Processor/GameProcessor.h"
#include "NetworkModule/JHNETTool.h"

using namespace std;
using namespace JHNETTool;
using namespace JHNETSerializer;

CRoom::CRoom()
{
	ServerNetworkSystem = CServerNetworkSystem::GetInstance();
}

void CRoom::Init(CPlayer* player, const UINT64& newRoomNumber)
{
	players[0] = player;
	for (int i = 1; i < MAX_PLAYER; ++i) players[i] = nullptr;
	state = LOBBY;
	roomNumber = newRoomNumber;
}

CPlayer* CRoom::GetPlayer(int slot)
{
	if (slot < 0 || slot >= MAX_PLAYER) return nullptr;
	return players[slot];
}

void CRoom::SetPlayer(const int& slot, CPlayer* player)
{
	if (slot < 0 || slot >= MAX_PLAYER) return;
	players[slot] = player;
}

void CRoom::SetState(const EGameState& newState)
{
	state = newState;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i]) {
			players[i]->state = state;
		}
	}
}

bool CRoom::ExistPlayer(const int& slot)
{
	if (slot < 0 || slot >= MAX_PLAYER) return false;
	return players[slot] != nullptr;
}

void CRoom::SwapPlayer(const int& slot1, const int& slot2)
{
	if (slot1 < 0 || slot1 >= MAX_PLAYER || slot2 < 0 || slot2 >= MAX_PLAYER || slot1 == slot2) return;

	CPlayer* temp = players[slot1];
	players[slot1] = players[slot2];
	players[slot2] = temp;
}

void CRoom::DisconnectPlayer(CPlayer* player)
{
	if (!player) return;

	// ���� ���̶�� : Disconnect�θ� ó���ϰ�, �����Ͷ�� �ٸ� ����� �����ͷ� �����. �׸��� Disconnect�� �뺸�Ѵ�.
	// ���� ���� �ƴ϶�� : ���� �ִ� ��Ƽ�� null�� ����ش�. �׸��� ��������� �뺸�Ѵ�.

	if (state == GAME)
	{
		// �����Ͷ��? 1�� ����� �����ͷ� �ٲپ� ������.
		if (players[0] == player) {
			SwapPlayer(0, 1);
		}
		for (int i = 0; i < MAX_PLAYER; ++i) {
			auto roomPlayer = players[i];
			if (roomPlayer == player) {
				roomPlayer->state = ConnectionLost;
				roomPlayer->RemoveUDPAddr();
				break;
			}
		}
		SendRoomInfoToAllMember();
		SendDisconnectToAllMember(player);
	}
	else {
		for (int i = 0; i < MAX_PLAYER; ++i) {
			auto player_i = players[i];
			if (player_i == player) {
				// �� ������ ��ܼ� ä���.
				for (int j = i; j < MAX_PLAYER; ++j) {
					// ������ �����̰ų�, ���� ������ ����ٸ� ���� ������ ����.
					if (j == MAX_PLAYER - 1 || players[j + 1] == nullptr) players[j] = nullptr;
					// �ƴ϶�� �ڿ��� ��ܿ´�.
					else players[j] = players[j + 1];
				}
				// Ȥ�ö� �ߺ��� ����� ������ �𸣴�, �ٽ� üũ�غ������� continue.
				--i;
				continue;
			}
		}

		// ������ �ο��鿡�� ���� ������ �˷��ش�.
		SendRoomInfoToAllMember();
		GameProcessor->RoomManager->CheckAndRestateRoom(this);
	}
}

bool CRoom::CanMatching()
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] && players[i]->state == LOBBY) {
			return false;
		}
	}
	return true;
}

bool CRoom::SetPlayerWhenEmpty(const int& slot, CPlayer* player)
{
	if (slot < 0 || slot >= MAX_PLAYER) return false;
	if (!players[slot]) {
		players[slot] = player;
		return true;
	}
	return false;
}

void CRoom::ChangePartyKing(int targetSlot)
{
	if (targetSlot == 0) return;
	CPlayer* temp = players[0];
	players[0] = players[targetSlot];
	players[targetSlot] = temp;
}

void CRoom::SendToAllMember(const char * buf, const int & len, const int& flag, bool isReliable)
{
	if (!players) return;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		CPlayer* player = players[i];
		if (player != nullptr && player->state != EGameState::ConnectionLost) {
			if (isReliable || !player->IsHaveUDP()) {
				GameProcessor->Send(player->socket, buf, len);
			}
			else {
				GameProcessor->SendTo(player->udpAddr, buf, len);
			}
		}
	}
}

void CRoom::SendToAllMember(FPacketStruct* packetStruct, const int& flag /*= 0*/, bool isReliable /*= true*/)
{
	char* buf = GameProcessor->bufObjectPool->PopObject();
	packetStruct->Serialize(buf);
	SendToAllMember(buf, (int)packetStruct->GetSize(), flag, isReliable);
	GameProcessor->bufObjectPool->ReturnObject(buf);
}

void CRoom::SendToOtherMember(const UINT64 member, const char * buf, const int & len, const int& flag, bool isReliable)
{
	if (!players) return;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		CPlayer* player = players[i];
		if (player != nullptr && player->steamID != member && player->state != EGameState::ConnectionLost) {
			if (isReliable || !player->IsHaveUDP()) {
				GameProcessor->Send(player->socket, buf, len);
			}
			else {
				GameProcessor->SendTo(player->udpAddr, buf, len);
			}
		}

	}
}

void CRoom::SendToOtherMember(const UINT64 member, FPacketStruct* packetStruct, const int& flag /*= 0*/, bool isReliable /*= true*/)
{
	char* buf = GameProcessor->bufObjectPool->PopObject();
	packetStruct->Serialize(buf);
	SendToOtherMember(member, buf, (int)packetStruct->GetSize(), flag, isReliable);
	GameProcessor->bufObjectPool->ReturnObject(buf);
}

void CRoom::SendRoomInfoToAllMember()
{
	WriteLog(ELogLevel::Warning, CLog::Format("[CRoom::SendRoomInfoToAllMember]\n"));

	// �� ����� ID�ڵ� MAX_PLAYER���� ���� ���۸� �����.
	FS_Room_Info sRoomInfo;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] != nullptr) sRoomInfo.slot[i] = players[i]->steamID;
		else sRoomInfo.slot[i] = 0;
	}

	// ����÷��̾� �鿡�� ���۸� ������.
	SendToAllMember((FPacketStruct*)&sRoomInfo);
}

void CRoom::SendRoomInfo(CPlayer* targetPlayer)
{
	if (!targetPlayer) return;
	WriteLog(ELogLevel::Warning, CLog::Format("[CRoom::SendRoomInfo]\n"));

	// �� ����� ID�ڵ� MAX_PLAYER���� ���� ���۸� �����.
	FS_Room_Info sRoomInfo;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] != nullptr) sRoomInfo.slot[i] = players[i]->steamID;
		else sRoomInfo.slot[i] = 0;
	}

	// ����÷��̾� �鿡�� ���۸� ������.
	GameProcessor->Send(targetPlayer->socket, (FPacketStruct*)& sRoomInfo);
}

// ���� ��� ������� �ش� ������ ������ �뺸�Ѵ�.
void CRoom::SendDisconnectToAllMember(CPlayer* disconnectedMember)
{
	if (!disconnectedMember) return;

	WriteLog(ELogLevel::Warning, CLog::Format("[CRoom::SendDisconnectToAllMember]\n"));

	// �ش� ����� ������ ��Ƽ� ������.
	FS_Disconnect_Slot sDisconnectSlot;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] == disconnectedMember) {
			sDisconnectSlot.slot = i;
			break;
		}
	}
	
	// �ٸ� �÷��̾� �鿡�� ���۸� ������.
	SendToOtherMember(disconnectedMember->steamID, (FPacketStruct*)&sDisconnectSlot);
}

// ���� ��� ������� �ش� ������ �������� �뺸�Ѵ�.
void CRoom::SendReconnectToAllMember(CPlayer* reconnectedMember)
{
	// ���� ���� �����鿡�� �ش� ������ ������ ������.
	{
		WriteLog(ELogLevel::Warning, CLog::Format("[CRoom::SendReconnectToAllMember]\n"));

		// �ش� ����� ������ ��Ƽ� ������.
		FS_Reconnect_Slot sReconnectSlot;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (players[i] == reconnectedMember) {
				sReconnectSlot.slot = i;
				break;
			}
		}

		// �ٸ� �÷��̾� �鿡�� ���۸� ������.
		SendToOtherMember(reconnectedMember->steamID, (FPacketStruct*)&sReconnectSlot);
	}

	// ����������Դ� �������� �뺸�ϰ� ���� ������ �ٽ� �������ش�.
	{
		// �� ����� ID�ڵ� MAX_PLAYER���� ���� ���۸� �����.
		FS_Reconnect sReconnect;
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (players[i] != nullptr) sReconnect.slot[i] = players[i]->steamID;
			else sReconnect.slot[i] = 0;
		}
		
		// �������� �������� ���� ������ �뺸���ش�.
		GameProcessor->Send(reconnectedMember->socket, (FPacketStruct*)&sReconnect);
	}
}

CPlayer* CRoom::JoinRoom(CPlayer* player)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] == nullptr) {
			//printf("Room JOIN SUCC!!!!\n");
			players[i] = player;
			return player;
		}
		if (players[i]->steamID == player->steamID) {
			//printf("Room ABSORB SUCC!!!!\n");
			GameProcessor->AbsorbPlayer(player, players[i]);
			return players[i];
		}
	}
	//printf("Room Join Failed!!!!\n");
	return player;
}

int CRoom::GetPlayerCount()
{
	int count = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] != nullptr) ++count;
	}
	return count;
}

int CRoom::GetNotLostedPlayerCount()
{
	int count = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		auto player = players[i];
		if (player && player->state != ConnectionLost) ++count;
	}
	return count;
}

void CRoom::DeletePlayer(const int& slot, bool bWithPushing)
{
	if (slot < 0 || slot >= MAX_PLAYER) return;

	if (bWithPushing) {
		// �� ������ ��ܼ� ä���.
		for (int i = slot; i < MAX_PLAYER; ++i) {
			// ������ �����̰ų�, ���� ������ ����ٸ� ���� ������ ����.
			if (i == MAX_PLAYER - 1 || players[i + 1] == nullptr) players[i] = nullptr;
			// �ƴ϶�� �ڿ��� ��ܿ´�.
			else players[i] = players[i + 1];
		}
	}
	else players[slot] = nullptr;
}

bool CRoom::FindPlayer(CPlayer* player)
{
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (players[i] == player) return true;
	}
	return false;
}

void CRoom::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::Room, level, msg);
}

void CRoom::SetGameProcessor(CGameProcessor* gameProcessor)
{
	GameProcessor = gameProcessor;
}
