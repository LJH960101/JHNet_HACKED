#pragma once
/*
	방에 대한 내용을 관할하는 클래스
*/

#include "JHNETGameServer.h"
#include "NetworkModule/NetworkData.h"
#include "PlayerManager.h"
#include <list>
#include <set>

using std::list;
using std::set;
class CRoom;

const UINT64 MAX_ROOMCOUNT = 18446744073709551;
class CRoomManager
{
public:
	CRoomManager();
	~CRoomManager();

	// innerMember의 룸을 찾아서, outterMember를 넣어준다.
	// 룸은 자동으로 나가짐.
	// 리턴값은 성공 여부.
	bool MoveRoom(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber);

	// 새로운 방을 만든다.
	// (새로운 유저가 접속했을때)
	CRoom* CreateRoom(CPlayer* member);

	// 플레이어를 추방시킨다음 새로운 방에 넣어준다.
	void KickPlayer(CPlayer* targetPlayer);

	CRoom* GetRoom(CPlayer* innerMember);

	// 연결상태인 룸을 찾는다.
	CRoom* GetConnectedRoom(CPlayer* innerMember);

	// Room을 Ready 상태로 바꿉니다.
	void ChangeRoomReady(CPlayer* player, const bool& isOn);

	int GetPlayerCount();
	int GetMatchRoomCount();
	int GetGameRoomCount();

	// 룸에서 단절된 플레이어를 알린다.
	void DisconnectRoom(CPlayer* player);

	// 한 ServerNetworkSystem Process Thread에 한번씩 호출되는 함수.
	void Update();

	// 강제로 룸에 조인시켜준다. DEBUG ONLY
	void ForceJoinRoom(CPlayer* targetPlayer);

	// 강제로 게임중인 룸에 조인시켜준다. DEBUG ONLY
	bool ForceJoinGameRoom(CPlayer* targetPlayer);

	// 강제로 방의 상태를 바꾼다. DEBUG ONLY
	void ForceChangeGameState(CRoom* room);
	CRoom* GetGameRoom(CPlayer* player);
	
	// 모든 방에서 나가고,
	// 이를 모든 클라이언트에게 알린다.
	// * 위험한 행동임.
	void OutRoom(CPlayer* innerMember);

	// 룸의 스테이트를 바꾼다.
	// 스테이트가 바뀌면서 리스트도 재정리 된다.
	void ChangeRoomState(CRoom* room, const EGameState& state);

	// 룸의 상태를 확인한다.
	void CheckAndRestateRoom(CRoom* room);

	void Print();
	void PrintObjectPools();
	void PrintSummary();

	void SetGameProcessor(class CGameProcessor* gameProcessor);

private:
	class CGameProcessor* GameProcessor;
	class CServerNetworkSystem* ServerNetworkSystem;

	int GetRoomMemberCount(CPlayer* innerMember);
	int GetRoomMemberCount(CRoom* room);

	void DeleteRoom(const UINT64& roomNumb);
	void DeleteRoom(CRoom* room);

	// 플레이 가능한 다른 방을 찾아본다.
	bool TryMatching(const int& memberCount, set<CRoom*>& outRooms);

	UINT64 roomCounter;
	list<CRoom*> rooms;
	list<CRoom*> matchRooms;
	list<CRoom*> gameRooms;

	static CRoomManager* _instance;

	static void WriteLog(const ELogLevel& level, const std::string& msg);

	// 방 이동을 수행한다.
	// 성공시 true 리턴
	bool _MoveRoomProc(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber);

	// 비동기를 지키기위한 뮤텍스.
	CObjectPool<CRoom>* roomObjectPool = nullptr;
};