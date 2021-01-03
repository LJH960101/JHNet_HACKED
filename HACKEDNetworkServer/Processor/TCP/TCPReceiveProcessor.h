#pragma once
/*
	TCP 데이터 수신 로직만을 담은 클래스
*/

#include "JHNETGameServer.h"
#include <string>
#include "NetworkModule/Log.h"
#include "Processor/DataType/Overlapped.h"

class CPlayer;
class CTCPReceiveProcessor
{
public:
	CTCPReceiveProcessor();
	~CTCPReceiveProcessor();

	// return false : ERROR
	bool ReceiveData(CPlayer*& player, char* recvBuf, int receiveLen);
	void SetGameProcessor(class CGameProcessor* gameProcessor);

private:
	class CGameProcessor* GameProcessor;
	class CServerNetworkSystem* ServerNetworkSystem;

	void WriteLog(const ELogLevel& level, const std::string& msg);

	// 메세지 처리부

	// Common
	void Common_Echo(CPlayer*& player, char* recvBuf, int& cursor);
	void Common_Ping(CPlayer*& player, char* recvBuf, int& cursor);
	void Common_AnswerId(CPlayer*& player, char* recvBuf, int& cursor);

	// Lobby
	void Lobby_InviteFriendRequest(CPlayer*& player, char* recvBuf, int& cursor);
	void Lobby_InviteFriendAnswer(CPlayer*& player, char* recvBuf, int& cursor);
	void Lobby_SetPartyKing(CPlayer*& player, char* recvBuf, int& cursor);
	void Lobby_FriendKickRequest(CPlayer*& player, char* recvBuf, int& cursor);
	void Lobby_MatchRequest(CPlayer*& player, char* recvBuf, int& cursor);

	// InGame
	void InGame_EndGame(CPlayer*& player, char* recvBuf, int& cursor);
	void InGame_RPC(CPlayer*& player, char* recvBuf, int& cursor);
	void InGame_SyncVar(CPlayer*& player, char* recvBuf, int& cursor);
	void InGame_ReconnectServer(CPlayer*& player, char* recvBuf, int& cursor);

	// Debug
	void Debug_RoomStart(CPlayer*& player, char* recvBuf, int& cursor);
	void Debug_GameStart(CPlayer*& player, char* recvBuf, int& cursor);
};

