#pragma once
/*
	�濡 ���� ������ �����ϴ� Ŭ����
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

	// innerMember�� ���� ã�Ƽ�, outterMember�� �־��ش�.
	// ���� �ڵ����� ������.
	// ���ϰ��� ���� ����.
	bool MoveRoom(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber);

	// ���ο� ���� �����.
	// (���ο� ������ ����������)
	CRoom* CreateRoom(CPlayer* member);

	// �÷��̾ �߹��Ų���� ���ο� �濡 �־��ش�.
	void KickPlayer(CPlayer* targetPlayer);

	CRoom* GetRoom(CPlayer* innerMember);

	// ��������� ���� ã�´�.
	CRoom* GetConnectedRoom(CPlayer* innerMember);

	// Room�� Ready ���·� �ٲߴϴ�.
	void ChangeRoomReady(CPlayer* player, const bool& isOn);

	int GetPlayerCount();
	int GetMatchRoomCount();
	int GetGameRoomCount();

	// �뿡�� ������ �÷��̾ �˸���.
	void DisconnectRoom(CPlayer* player);

	// �� ServerNetworkSystem Process Thread�� �ѹ��� ȣ��Ǵ� �Լ�.
	void Update();

	// ������ �뿡 ���ν����ش�. DEBUG ONLY
	void ForceJoinRoom(CPlayer* targetPlayer);

	// ������ �������� �뿡 ���ν����ش�. DEBUG ONLY
	bool ForceJoinGameRoom(CPlayer* targetPlayer);

	// ������ ���� ���¸� �ٲ۴�. DEBUG ONLY
	void ForceChangeGameState(CRoom* room);
	CRoom* GetGameRoom(CPlayer* player);
	
	// ��� �濡�� ������,
	// �̸� ��� Ŭ���̾�Ʈ���� �˸���.
	// * ������ �ൿ��.
	void OutRoom(CPlayer* innerMember);

	// ���� ������Ʈ�� �ٲ۴�.
	// ������Ʈ�� �ٲ�鼭 ����Ʈ�� ������ �ȴ�.
	void ChangeRoomState(CRoom* room, const EGameState& state);

	// ���� ���¸� Ȯ���Ѵ�.
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

	// �÷��� ������ �ٸ� ���� ã�ƺ���.
	bool TryMatching(const int& memberCount, set<CRoom*>& outRooms);

	UINT64 roomCounter;
	list<CRoom*> rooms;
	list<CRoom*> matchRooms;
	list<CRoom*> gameRooms;

	static CRoomManager* _instance;

	static void WriteLog(const ELogLevel& level, const std::string& msg);

	// �� �̵��� �����Ѵ�.
	// ������ true ����
	bool _MoveRoomProc(CPlayer* innerMember, CPlayer* outterMember, int& outSlotNumber);

	// �񵿱⸦ ��Ű������ ���ؽ�.
	CObjectPool<CRoom>* roomObjectPool = nullptr;
};