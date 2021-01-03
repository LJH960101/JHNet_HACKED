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

	// 접속자가 이미 파티중이라면 이동하지 않는다.
	CRoom* outterRoom = GetRoom(outterMember);
	if (!outterRoom && GetRoomMemberCount(outterRoom) != 1) return false;

	CRoom* room = GetRoom(innerMember);
	if (GetRoomMemberCount(room) < MAX_PLAYER) {
		// 방 접속 가능
		// 기존방에서 나간다.
		OutRoom(outterMember);
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (room->SetPlayerWhenEmpty(i, outterMember)) {
				outSlotNumber = i;

				// 방접속 성공!
				return true;
			}
		}
		// 방 접속에 실패했다면, 방을 다시 만들어준다.
		CreateRoom(outterMember);
	}

	// 방 접속 실패 !!!
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

	// 강퇴 된 이후의 방 정보를 알려준다.
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
	// 로비/매칭 레디 상태에선 상호 교환이 일어난다.
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
		// 이미 집합에 존재하는 멤버면 무시한다.
		if (outRooms.find(i) != outRooms.end()) continue;

		// 플레이어 카운트를 합산해 다시 계산.
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
		// 플레이어를 찾음
		if (room->GetPlayer(i) == player) {
			if(isOn) player->state = READY;
			else player->state = LOBBY;

			FS_Lobby_MatchAnswer lobbyMatchAsnwer;
			lobbyMatchAsnwer.isOn = isOn;
			lobbyMatchAsnwer.slot = i;

			// 갱신 정보를 전송
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

	// 회원이 접속중인 룸을 조회
	if (room) {
		int count = room->GetNotLostedPlayerCount();

		// 혼자 있는 방은 삭제
		if (count <= 1) {
			DeleteRoom(room->GetRoomNumber());

			// 플레이어도 내쫒는다.
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

			// 혹시라도 룸이 비어버렸다면 다시한번 삭제처리를 한다.
			if (room->GetNotLostedPlayerCount() == 0) {
				DeleteRoom(room->GetRoomNumber());

				// 플레이어도 내쫒는다.
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
	// 소속된 방이 없다면 플레이어를 그냥 삭제한다.
	else {
		GameProcessor->PlayerManager->RemovePlayer(player);
	}
}

void CRoomManager::Update()
{
	// 매칭을 시켜준다.
	set<CRoom*> matchingSuccessRooms;
	for (auto i : matchRooms) {
		matchingSuccessRooms.insert(i);
		int count = i->GetPlayerCount();
		if (TryMatching(count, matchingSuccessRooms)) {
			matchingSuccessRooms.erase(i);
			// i의 룸으로 모두 이전 시킨다.
			int emptySlot = 0;
			for (int i_slot = 0; i_slot < MAX_PLAYER; ++i_slot) {
				if (!i->ExistPlayer(i_slot)) {
					emptySlot = i_slot;
					break;
				}
			}
			// 빈곳을 집합에서 한명씩 넣어준다.
			for (auto matchedRoom : matchingSuccessRooms) {
				for (int matchedRoom_slot = 0; matchedRoom_slot < MAX_PLAYER; ++matchedRoom_slot) {
					// 매칭된 룸에서 사람이 존재한다면 이동.
					if (matchedRoom->ExistPlayer(matchedRoom_slot)) {
						if(emptySlot<MAX_PLAYER) i->SetPlayer(emptySlot++, matchedRoom->GetPlayer(matchedRoom_slot));
						matchedRoom->DeletePlayer(matchedRoom_slot);
					}
				}
				// 이후 매칭된 방을 파괴시킨다.
				DeleteRoom(matchedRoom);
			}

			// 룸의 상태를 갱신시켜주고, 게임 시작을 알린다.
			WriteLog(ELogLevel::Warning, CLog::Format("[%llu room] Game Start!\n", i->GetRoomNumber()));
			ChangeRoomState(i, GAME);
			i->SendRoomInfoToAllMember();
			FS_Lobby_GameStart sLobbyGameStart;
			i->SendToAllMember((FPacketStruct*)&sLobbyGameStart);

			// 성공했다면 처음부터 다시 진행한다.
			Update();
			return;
		}
		matchingSuccessRooms.clear();
	}
	// 빈 방을 삭제시켜준다.
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

			// 이미 속한 방이 아니라면 접속하고 통보.
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

			// 이미 속한 방이 아니라면 접속하고 통보.
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
	// 회원이 접속중인 모든 룸을 조회
	while (room) {
		int count = GetRoomMemberCount(room);
		// 혼자 있는 방은 삭제
		if(count == 1) DeleteRoom(room->GetRoomNumber());
		// 같이 있다면 null로 비워준다.
		else {
			for (int i = 0; i < MAX_PLAYER; ++i) {
				auto player_i = room->GetPlayer(i);
				if (player_i == innerMember) {
					room->DeletePlayer(i, true);
					break;
				}
			}

			// 나머지 인원들에게 변경 사항을 알려준다.
			room->SendRoomInfoToAllMember();
			CheckAndRestateRoom(room);

			// 혹시라도 룸이 비어버렸다면 다시한번 삭제처리를 한다.
			if (GetRoomMemberCount(room) == 0) DeleteRoom(room->GetRoomNumber());
		}
		room = GetRoom(innerMember);
	}
}

bool CRoomManager::MoveRoom(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber)
{
	// 룸이동 성공?
	if (_MoveRoomProc(innerMember, outterMember, outSlotNumber)) {
		CRoom* innerRoom = GetRoom(innerMember);
		if (innerRoom) innerRoom->SendRoomInfoToAllMember();
		WriteLog(ELogLevel::Warning, "Success to move.\n");
		return true;
	}
	// 룸이동 실패?
	else {
		char msg[] = "방 이동에 실패하였습니다.";

		FS_Lobby_InviteFriend_Failed sLobbyInviteFriendFailed;
		memcpy(sLobbyInviteFriendFailed.msg.buf, msg, sizeof(msg));
		sLobbyInviteFriendFailed.msg.len = (int)sizeof(msg);

		if (outterMember) GameProcessor->Send(outterMember->socket, (FPacketStruct*)&sLobbyInviteFriendFailed);
		if (innerMember) GameProcessor->Send(innerMember->socket, (FPacketStruct*)&sLobbyInviteFriendFailed);
		WriteLog(ELogLevel::Warning, "Failed to move.\n");

		return false;
	}
}