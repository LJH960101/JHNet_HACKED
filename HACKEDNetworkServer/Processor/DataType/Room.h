#pragma once
/*
	���� ������ ��� Ŭ����
*/

#include "JHNETGameServer.h"
#include "NetworkModule/PacketStruct.h"
#include "Processor/Manager/RoomManager.h"
class CPlayer;

class CRoom
{
	//friend class CRoomManager;

public:
	CRoom();
	void Init(CPlayer* player, const UINT64& newRoomNumber);

	CPlayer* GetPlayer(int slot);
	void SetPlayer(const int& slot, CPlayer* player);
	void SetState(const EGameState& newState);
	EGameState GetState() { return state; }
	UINT64 GetRoomNumber() { return roomNumber; }
	bool ExistPlayer(const int& slot);
	
	// �ش� ������ ���� ó�� �Ѵ�.
	void DisconnectPlayer(CPlayer* slot);

	// ��� ��Ī�� �غ� �Ǿ��ִ��� �Ǵ��Ѵ�.
	bool CanMatching();

	// �÷��̾ �������� �ʴٸ� �ִ� �Լ�.
	bool SetPlayerWhenEmpty(const int& slot, CPlayer* player);

	// ��Ƽ���� ��ü�մϴ�.
	void ChangePartyKing(int targetSlot);

	// ���� ��� ������� �����Ѵ�.
	void SendToAllMember(FPacketStruct* packetStruct, const int& flag = 0, bool isReliable = true);
	void SendToAllMember(const char* buf, const int& len, const int& flag = 0, bool isReliable = true);

	// ���� Ư�� ����� �����ϰ� ��� �����Ѵ�.
	void SendToOtherMember(const UINT64 member, FPacketStruct* packetStruct, const int& flag = 0, bool isReliable = true);
	void SendToOtherMember(const UINT64 member, const char* buf, const int& len, const int& flag = 0, bool isReliable = true);

	// ���� ��� ������� ���� ������ �������ش�.
	void SendRoomInfoToAllMember();

	// ���� ������ �����մϴ�.
	void SendRoomInfo(CPlayer* targetPlayer);

	// ���� ��� ������� �ش� ������ ������ �뺸�Ѵ�.
	void SendDisconnectToAllMember(CPlayer* disconnectedMember);

	// ���� ��� ������� �ش� ������ �������� �뺸�Ѵ�.
	void SendReconnectToAllMember(CPlayer* reconnectedMember);

	// �濡 �����Ѵ�.
	CPlayer* JoinRoom(CPlayer* player);

	// ����� �÷��̾� ���� ���մϴ�.
	int GetPlayerCount();

	// ��ȿ� �÷������� �÷��̾ ���մϴ�.
	int GetNotLostedPlayerCount();

	// ����� �÷��̾ �����մϴ�.
	// withPushing�� True�� ä��� �۾��� �����մϴ�.
	void DeletePlayer(const int& slot, bool bWithPushing = false);

	// �� �÷��̾��� ��ġ�� �ٲ۴�.
	void SwapPlayer(const int& slot1, const int& slot2);

	bool FindPlayer(CPlayer* player);

	static void WriteLog(const ELogLevel& level, const std::string& msg);

	void SetGameProcessor(class CGameProcessor* GameProcessor);

private:
	CServerNetworkSystem* ServerNetworkSystem;
	class CGameProcessor* GameProcessor;

	UINT64 roomNumber;
	CPlayer* players[MAX_PLAYER]; // 0~3���� 4���� �÷��̾� ����
	EGameState state;
};