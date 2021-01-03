#pragma once
/*
	방의 정보를 담는 클래스
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
	
	// 해당 슬롯을 퇴장 처리 한다.
	void DisconnectPlayer(CPlayer* slot);

	// 모두 매칭할 준비가 되어있는지 판단한다.
	bool CanMatching();

	// 플레이어가 존재하지 않다면 넣는 함수.
	bool SetPlayerWhenEmpty(const int& slot, CPlayer* player);

	// 파티장을 교체합니다.
	void ChangePartyKing(int targetSlot);

	// 방의 모든 사람에게 전송한다.
	void SendToAllMember(FPacketStruct* packetStruct, const int& flag = 0, bool isReliable = true);
	void SendToAllMember(const char* buf, const int& len, const int& flag = 0, bool isReliable = true);

	// 방의 특정 사람을 제외하고 모두 전송한다.
	void SendToOtherMember(const UINT64 member, FPacketStruct* packetStruct, const int& flag = 0, bool isReliable = true);
	void SendToOtherMember(const UINT64 member, const char* buf, const int& len, const int& flag = 0, bool isReliable = true);

	// 방의 모든 사람에게 방의 정보를 전송해준다.
	void SendRoomInfoToAllMember();

	// 방의 정보를 전송합니다.
	void SendRoomInfo(CPlayer* targetPlayer);

	// 방의 모든 사람에게 해당 유저의 단절을 통보한다.
	void SendDisconnectToAllMember(CPlayer* disconnectedMember);

	// 방의 모든 사람에게 해당 유저의 재접속을 통보한다.
	void SendReconnectToAllMember(CPlayer* reconnectedMember);

	// 방에 진입한다.
	CPlayer* JoinRoom(CPlayer* player);

	// 룸안의 플레이어 수를 구합니다.
	int GetPlayerCount();

	// 룸안에 플레이중인 플레이어를 구합니다.
	int GetNotLostedPlayerCount();

	// 룸안의 플레이어를 제거합니다.
	// withPushing이 True면 채우기 작업도 병행합니다.
	void DeletePlayer(const int& slot, bool bWithPushing = false);

	// 두 플레이어의 위치를 바꾼다.
	void SwapPlayer(const int& slot1, const int& slot2);

	bool FindPlayer(CPlayer* player);

	static void WriteLog(const ELogLevel& level, const std::string& msg);

	void SetGameProcessor(class CGameProcessor* GameProcessor);

private:
	CServerNetworkSystem* ServerNetworkSystem;
	class CGameProcessor* GameProcessor;

	UINT64 roomNumber;
	CPlayer* players[MAX_PLAYER]; // 0~3까지 4명의 플레이어 존재
	EGameState state;
};