#include "RoomManager.h"
#include "Processor/DataType/Room.h"
#include "NetworkModule/PacketStruct.h"
#include "Server/ServerNetworkSystem.h"
#include "NetworkModule/JHNETTool.h"
#include "NetworkModule/Log.h"
#include "Processor/GameProcessor.h"
#include <string.h>
#include <algorithm>

using namespace std;
using namespace JHNETTool;
using namespace JHNETSerializer;

void CRoomManager::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::RoomManager, level, msg);
}

bool CRoomManager::_MoveRoomProc(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber)
{
	if (innerMember == nullptr) return false;

	// �����ڰ� �̹� ��Ƽ���̶�� �̵����� �ʴ´�.
	CRoom* outterRoom = GetRoom(outterMember);
	if (!outterRoom && GetRoomMemberCount(outterRoom) != 1) return false;

	CRoom* room = GetRoom(innerMember);
	if (GetRoomMemberCount(room) < MAX_PLAYER) {
		// �� ���� ����
		// �����濡�� ������.
		OutRoom(outterMember);
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (room->SetPlayerWhenEmpty(i, outterMember)) {
				outSlotNumber = i;

				// ������ ����!
				return true;
			}
		}
		// �� ���ӿ� �����ߴٸ�, ���� �ٽ� ������ش�.
		CreateRoom(outterMember);
	}

	// �� ���� ���� !!!
	return false;
}

CRoomManager::CRoomManager()
{
	roomCounter = 1;
	roomObjectPool = new CObjectPool<CRoom>();
	roomObjectPool->SetWithMutex(false);
	ServerNetworkSystem = CServerNetworkSystem::GetInstance();
}

CRoomManager::~CRoomManager()
{
	if (roomObjectPool) delete roomObjectPool;
}

CRoom* CRoomManager::CreateRoom(CPlayer* member)
{
	CRoom* newRoom = roomObjectPool->PopObject();
	newRoom->SetGameProcessor(GameProcessor);
	newRoom->Init(member, roomCounter++);
	if (roomCounter == MAX_ROOMCOUNT) roomCounter = 1;
	
	rooms.push_back(newRoom);

	return newRoom;
}

void CRoomManager::KickPlayer(CPlayer* targetPlayer)
{
	if (!targetPlayer) return;

	OutRoom(targetPlayer);
	CreateRoom(targetPlayer);
	CRoom* kickPlayerRoom = GetRoom(targetPlayer);

	// ���� �� ������ �� ������ �˷��ش�.
	if (kickPlayerRoom) kickPlayerRoom->SendRoomInfoToAllMember();
}

CRoom* CRoomManager::GetRoom(CPlayer* innerMember)
{
	if (innerMember == nullptr) return nullptr;
	for (auto i = rooms.begin(); i != rooms.end(); ++i) {
		for (int j = 0; j < MAX_PLAYER; ++j) {
			if ((*i)->GetPlayer(j) == innerMember) {
				return *i;
			}
		}
	}
	return nullptr;
}

CRoom* CRoomManager::GetConnectedRoom(CPlayer* innerMember)
{
	if (innerMember == nullptr) return nullptr;
	for (auto i = rooms.begin(); i != rooms.end(); ++i) {
		for (int j = 0; j < MAX_PLAYER; ++j) {
			auto player = (*i)->GetPlayer(j);
			if (player && player == innerMember && player->state != EGameState::ConnectionLost) {
				return *i;
			}
		}
	}
	return nullptr;
}

void CRoomManager::DeleteRoom(const UINT64& roomNumb)
{
	for (list<CRoom*>::iterator it = rooms.begin();
		it != rooms.end();)
	{
		if ((*it)->GetRoomNumber() == roomNumb) {
			CRoom* ptr = *it;
			DeleteRoom(ptr);
			break;
		}
		else
			++it;
	}
}

void CRoomManager::DeleteRoom(CRoom* room)
{
	rooms.remove(room);
	matchRooms.remove(room);
	gameRooms.remove(room);
	roomObjectPool->ReturnObject(room);
}

void CRoomManager::ChangeRoomState(CRoom* room, const EGameState& state)
{
	if (!room) return;
	if (room->GetState() == state) {
		return;
	}
	switch (state)
	{
	case LOBBY:
		if (room->GetState() == READY) {
			matchRooms.remove(room);
		}
		else if (room->GetState() == GAME) {
			gameRooms.remove(room);
		}
		break;
	case READY:
			matchRooms.push_back(room);
		break;
	case GAME:
			matchRooms.remove(room);
			gameRooms.push_back(room);
		break;
	default:
		break;
	}
	room->SetState(state);
}

void CRoomManager::CheckAndRestateRoom(CRoom* room){
	if (!room) return;
	// �κ�/��Ī ���� ���¿��� ��ȣ ��ȯ�� �Ͼ��.
	if (room->GetState() == LOBBY || room->GetState() == READY) {
		if (room->CanMatching()) {
			ChangeRoomState(room, READY);
		}
		else {
			ChangeRoomState(room, LOBBY);
		}
	}
}

void CRoomManager::Print()
{
	int count = 0;
	printf("Room Print : \n");
	for (auto i : rooms) {
		printf("%d : ", count++);
		for (int j = 0; j < MAX_PLAYER; ++j) {
			if (i->GetPlayer(j)) printf("%llu(%p) ", (i->GetPlayer(j)->steamID), i->GetPlayer(j));
			else printf("0 ");
		}
		printf("\n");
	}
}

void CRoomManager::PrintObjectPools()
{
	printf("CRoomManager::PrintObjectPools\n");
	roomObjectPool->Print();
}

void CRoomManager::PrintSummary()
{
	std::cout << "R" << GetPlayerCount() << " "
		<< "MR" << GetMatchRoomCount() << " "
		<< "GR" << GetGameRoomCount() << " ";
}

void CRoomManager::SetGameProcessor(class CGameProcessor* gameProcessor)
{
	GameProcessor = gameProcessor;
}

CRoom* CRoomManager::GetGameRoom(CPlayer* player)
{
	CRoom* playerRoom = GetRoom(player);
	if (playerRoom == nullptr) return nullptr;

	auto it = std::find(gameRooms.begin(), gameRooms.end(), playerRoom);
	if (it != gameRooms.end()) { // founded!!
		return playerRoom;
	}
	else { // not founded.
		return nullptr;
	}
}

bool CRoomManager::TryMatching(const int& memberCount, set<CRoom*>& outRooms)
{
	if (memberCount == MAX_PLAYER) return true;
	for (auto i : matchRooms) {
		// �̹� ���տ� �����ϴ� ����� �����Ѵ�.
		if (outRooms.find(i) != outRooms.end()) continue;

		// �÷��̾� ī��Ʈ�� �ջ��� �ٽ� ���.
		int count = i->GetPlayerCount();

		if (count > 0 && count + memberCount <= MAX_PLAYER) {
			outRooms.insert(i);
			if(TryMatching(memberCount + count, outRooms)) return true;
			outRooms.erase(i);
		}
	}
	return false;
}

void CRoomManager::ChangeRoomReady(CPlayer* player, const bool& isOn)
{
	CRoom* room = GetRoom(player);
	if (!room) return;
	if (room->GetState() == GAME) return;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		// �÷��̾ ã��
		if (room->GetPlayer(i) == player) {
			if(isOn) player->state = READY;
			else player->state = LOBBY;

			FS_Lobby_MatchAnswer lobbyMatchAsnwer;
			lobbyMatchAsnwer.isOn = isOn;
			lobbyMatchAsnwer.slot = i;

			// ���� ������ ����
			for (int j = 0; j < MAX_PLAYER; ++j) {
				auto player_j = room->GetPlayer(j);
				if (player_j != nullptr) {
					GameProcessor->Send(player_j->socket, (FPacketStruct*)&lobbyMatchAsnwer);
				}
			}

			CheckAndRestateRoom(room);
			return;
		}
	}
}

int CRoomManager::GetPlayerCount() {
	return static_cast<int>(rooms.size());
}

int CRoomManager::GetMatchRoomCount()
{
	return static_cast<int>(matchRooms.size());
}

int CRoomManager::GetGameRoomCount()
{
	return static_cast<int>(gameRooms.size());
}

void CRoomManager::DisconnectRoom(CPlayer* player)
{
	CRoom* room = GetConnectedRoom(player);

	// ȸ���� �������� ���� ��ȸ
	if (room) {
		int count = room->GetNotLostedPlayerCount();

		// ȥ�� �ִ� ���� ����
		if (count <= 1) {
			DeleteRoom(room->GetRoomNumber());

			// �÷��̾ ���i�´�.
			for (int j = 0; j < MAX_PLAYER; ++j) {
				auto player_j = room->GetPlayer(j);
				if (player_j) {
					room->DeletePlayer(j);
					GameProcessor->PlayerManager->RemovePlayer(player_j);
				}
			}
		}
		else {
			room->DisconnectPlayer(player);

			// Ȥ�ö� ���� �����ȴٸ� �ٽ��ѹ� ����ó���� �Ѵ�.
			if (room->GetNotLostedPlayerCount() == 0) {
				DeleteRoom(room->GetRoomNumber());

				// �÷��̾ ���i�´�.
				for (int j = 0; j < MAX_PLAYER; ++j) {
					auto player_j = room->GetPlayer(j);
					if (player_j) {
						room->DeletePlayer(j);
						GameProcessor->PlayerManager->RemovePlayer(player_j);
					}
				}
			}
		}
	}
	// �Ҽӵ� ���� ���ٸ� �÷��̾ �׳� �����Ѵ�.
	else {
		GameProcessor->PlayerManager->RemovePlayer(player);
	}
}

void CRoomManager::Update()
{
	// ��Ī�� �����ش�.
	set<CRoom*> matchingSuccessRooms;
	for (auto i : matchRooms) {
		matchingSuccessRooms.insert(i);
		int count = i->GetPlayerCount();
		if (TryMatching(count, matchingSuccessRooms)) {
			matchingSuccessRooms.erase(i);
			// i�� ������ ��� ���� ��Ų��.
			int emptySlot = 0;
			for (int i_slot = 0; i_slot < MAX_PLAYER; ++i_slot) {
				if (!i->ExistPlayer(i_slot)) {
					emptySlot = i_slot;
					break;
				}
			}
			// ����� ���տ��� �Ѹ� �־��ش�.
			for (auto matchedRoom : matchingSuccessRooms) {
				for (int matchedRoom_slot = 0; matchedRoom_slot < MAX_PLAYER; ++matchedRoom_slot) {
					// ��Ī�� �뿡�� ����� �����Ѵٸ� �̵�.
					if (matchedRoom->ExistPlayer(matchedRoom_slot)) {
						if(emptySlot<MAX_PLAYER) i->SetPlayer(emptySlot++, matchedRoom->GetPlayer(matchedRoom_slot));
						matchedRoom->DeletePlayer(matchedRoom_slot);
					}
				}
				// ���� ��Ī�� ���� �ı���Ų��.
				DeleteRoom(matchedRoom);
			}

			// ���� ���¸� ���Ž����ְ�, ���� ������ �˸���.
			WriteLog(ELogLevel::Warning, CLog::Format("[%llu room] Game Start!\n", i->GetRoomNumber()));
			ChangeRoomState(i, GAME);
			i->SendRoomInfoToAllMember();
			FS_Lobby_GameStart sLobbyGameStart;
			i->SendToAllMember((FPacketStruct*)&sLobbyGameStart);

			// �����ߴٸ� ó������ �ٽ� �����Ѵ�.
			Update();
			return;
		}
		matchingSuccessRooms.clear();
	}
	// �� ���� ���������ش�.
	auto it = rooms.begin();
	while (it != rooms.end())
	{
		bool isLost = true;
		for (int j = 0; j < MAX_PLAYER; ++j) {
			auto player_j = (*it)->GetPlayer(j);
			if (player_j && player_j->state != EGameState::ConnectionLost) {
				isLost = false;
				break;
			}
		}

		if (isLost) {
			for (int j = 0; j < MAX_PLAYER; ++j) {
				auto player_j = (*it)->GetPlayer(j);
				if (player_j) {
					GameProcessor->PlayerManager->RemovePlayer(player_j);
					(*it)->DeletePlayer(j);
				}
			}
			CRoom* ptr = *it;
			DeleteRoom(ptr);
			it++;
		}
		else it++;
	}
}

void CRoomManager::ForceJoinRoom(CPlayer* targetPlayer)
{
	for (auto i : rooms) {
		int count = i->GetPlayerCount();
		if (count < MAX_PLAYER) {
			CRoom*  room = i;

			// �̹� ���� ���� �ƴ϶�� �����ϰ� �뺸.
			if (room->FindPlayer(targetPlayer)) continue;

			OutRoom(targetPlayer);
			room->JoinRoom(targetPlayer);
			room->SendRoomInfoToAllMember();
			return;
		}
	}
}

bool CRoomManager::ForceJoinGameRoom(CPlayer* targetPlayer)
{
	for (auto i : gameRooms) {
		int count = i->GetPlayerCount();
		if (count < MAX_PLAYER) {
			CRoom*  room = i;

			// �̹� ���� ���� �ƴ϶�� �����ϰ� �뺸.
			if (room->FindPlayer(targetPlayer)) continue;

			OutRoom(targetPlayer);
			room->JoinRoom(targetPlayer);
			room->SendRoomInfoToAllMember();
			targetPlayer->state = GAME;
			return true;
		}
	}
	return false;
}

void CRoomManager::ForceChangeGameState(CRoom* room)
{
	ChangeRoomState(room, GAME);
}

int CRoomManager::GetRoomMemberCount(CPlayer* innerMember)
{
	CRoom* room = GetRoom(innerMember);
	if (room == nullptr) return 0;
	return GetRoomMemberCount(room);
}

int CRoomManager::GetRoomMemberCount(CRoom* room)
{
	return room->GetPlayerCount();
}

void CRoomManager::OutRoom(CPlayer* innerMember)
{
	CRoom* room = GetRoom(innerMember);
	// ȸ���� �������� ��� ���� ��ȸ
	while (room) {
		int count = GetRoomMemberCount(room);
		// ȥ�� �ִ� ���� ����
		if(count == 1) DeleteRoom(room->GetRoomNumber());
		// ���� �ִٸ� null�� ����ش�.
		else {
			for (int i = 0; i < MAX_PLAYER; ++i) {
				auto player_i = room->GetPlayer(i);
				if (player_i == innerMember) {
					room->DeletePlayer(i, true);
					break;
				}
			}

			// ������ �ο��鿡�� ���� ������ �˷��ش�.
			room->SendRoomInfoToAllMember();
			CheckAndRestateRoom(room);

			// Ȥ�ö� ���� �����ȴٸ� �ٽ��ѹ� ����ó���� �Ѵ�.
			if (GetRoomMemberCount(room) == 0) DeleteRoom(room->GetRoomNumber());
		}
		room = GetRoom(innerMember);
	}
}

bool CRoomManager::MoveRoom(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber)
{
	// ���̵� ����?
	if (_MoveRoomProc(innerMember, outterMember, outSlotNumber)) {
		CRoom* innerRoom = GetRoom(innerMember);
		if (innerRoom) innerRoom->SendRoomInfoToAllMember();
		WriteLog(ELogLevel::Warning, "Success to move.\n");
		return true;
	}
	// ���̵� ����?
	else {
		char msg[] = "�� �̵��� �����Ͽ����ϴ�.";

		FS_Lobby_InviteFriend_Failed sLobbyInviteFriendFailed;
		memcpy(sLobbyInviteFriendFailed.msg.buf, msg, sizeof(msg));
		sLobbyInviteFriendFailed.msg.len = (int)sizeof(msg);

		if (outterMember) GameProcessor->Send(outterMember->socket, (FPacketStruct*)&sLobbyInviteFriendFailed);
		if (innerMember) GameProcessor->Send(innerMember->socket, (FPacketStruct*)&sLobbyInviteFriendFailed);
		WriteLog(ELogLevel::Warning, "Failed to move.\n");

		return false;
	}
}