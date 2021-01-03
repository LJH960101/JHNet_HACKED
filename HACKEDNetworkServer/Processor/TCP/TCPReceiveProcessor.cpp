#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TCPReceiveProcessor.h"
#include "Server/ServerNetworkSystem.h"
#include "Processor/GameProcessor.h"
#include "Processor/Manager/PlayerManager.h"
#include "Processor/Manager/RoomManager.h"
#include "NetworkModule/PacketStruct.h"
#include "Processor/DataType/Room.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/Log.h"
#include "NetworkModule/JHNETTool.h"
#include "Processor/Manager/PlayerManager.h"
#include <stdio.h>
#include <chrono>
#include <ctime> 

using namespace JHNETTool;
using namespace std;
using namespace JHNETSerializer;

bool CTCPReceiveProcessor::ReceiveData(CPlayer*& player, char* recvBuf, int receiveLen)
{
	if (!player) {
		WriteLog(ELogLevel::Critical, CLog::Format("Critical Error!!! TCP Player not exist.."));
		return false;
	}
	// �÷��̾ ������.
	if (receiveLen == 0) {
		WriteLog(ELogLevel::Error, CLog::Format("ReceiveData : CloseConnection."));
		return false;
	}
	if (receiveLen < 0 || receiveLen > PACKET_BUFSIZE) {
		WriteLog(ELogLevel::Error, CLog::Format("ReceiveData : Wrong ReceiveLen"));
		return false;
	}

	// �����̵� ���۰� �ִ��� Ȯ���ϰ� �ִٸ� ������ ���� ���ۿ� �����ش�.
	if (player->delayedTCPDataLen > 0) {
		player->PopDelayData(recvBuf, receiveLen, true);
	}

	int cursor = 0, bufLen = 0;
	FPacketStruct packetStruct;
	while (cursor < receiveLen) {
		// ENUM�� ������ ���̸� ����Ѵ�.
		bufLen = IntDeserialize(recvBuf, &cursor);
		if (bufLen < 0) {
			WriteLog(ELogLevel::Error, CLog::Format("TCP Receive buflen < 0, bufSize = %d, cursor = %d, len = %d", bufLen, cursor, receiveLen));
			return false;
		}
		if (bufLen > PACKET_BUFSIZE) {
			WriteLog(ELogLevel::Error, CLog::Format("TCP Receive buflen > BUFSIZE, bufSize = %d, cursor = %d, len = %d", bufLen, cursor, receiveLen));
			return false;
		}

		// ���ŷ��� �Ѿ �����Ͱ� �Ѿ�´ٸ� ���� �����͸� �����ϰ� �����Ѵ�.
		if (cursor + bufLen > receiveLen) {
			cursor -= (int)sizeof(INT32);
			player->PushDelayData(recvBuf, cursor, receiveLen, true);
			return true;
		}

		int currentCursor = cursor;
		packetStruct.Deserialize(recvBuf, &currentCursor);

		std::string recvLog = CLog::Format("[%s:%d] : Recv type : %d\n",
			inet_ntoa(player->addr.sin_addr),
			ntohs(player->addr.sin_port), packetStruct.type);
		WriteLog(ELogLevel::Warning, recvLog);

		switch (packetStruct.type)
		{
		case EMessageType::Common_Echo:
		{
			Common_Echo(player, recvBuf, cursor);
			break;
		}
		case EMessageType::Common_Ping:
		{
			Common_Ping(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Common_AnswerId:
		{
			Common_AnswerId(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Lobby_InviteFriend_Request:
		{
			Lobby_InviteFriendRequest(player, recvBuf, cursor);
			break;
		}

		case EMessageType::C_Lobby_InviteFriend_Answer:
		{
			Lobby_InviteFriendAnswer(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Lobby_SetPartyKing:
		{
			Lobby_SetPartyKing(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Lobby_FriendKick_Request:
		{
			Lobby_FriendKickRequest(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Lobby_MatchRequest:
		{
			Lobby_MatchRequest(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Debug_RoomStart:
		{
			Debug_RoomStart(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Debug_GameStart:
		{
			Debug_GameStart(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_InGame_RPC:
		{
			InGame_RPC(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_InGame_SyncVar:
		{
			InGame_SyncVar(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_Reconnect_Server:
		{
			InGame_ReconnectServer(player, recvBuf, cursor);
			break;
		}
		case EMessageType::C_End_Game:
		{
			InGame_EndGame(player, recvBuf, cursor);
			break;
		}
		default:
			std::string logString = CLog::Format("Unknown receive type!!!! : %d", (int)packetStruct.type);
			WriteLog(ELogLevel::Error, logString);
			cursor += bufLen;
			break;
		}
	}
	return true;
}

void CTCPReceiveProcessor::SetGameProcessor(class CGameProcessor* gameProcessor)
{
	GameProcessor = gameProcessor;
}

void CTCPReceiveProcessor::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::TCPReceiveProcessor, level, msg);
}

CTCPReceiveProcessor::CTCPReceiveProcessor()
{
	ServerNetworkSystem = CServerNetworkSystem::GetInstance();
}


CTCPReceiveProcessor::~CTCPReceiveProcessor()
{
}

void CTCPReceiveProcessor::Common_Echo(CPlayer*& player, char* recvBuf, int& cursor)
{
	F_Common_Echo commonEcho;
	commonEcho.Deserialize(recvBuf, &cursor);
	
	printf("[%s:%d] : ECHO, %s\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port),
		commonEcho.newString.buf);
}

void CTCPReceiveProcessor::Common_Ping(CPlayer*& player, char* recvBuf, int& cursor)
{
	F_Common_Ping commonPing;
	commonPing.Deserialize(recvBuf, &cursor);

	// �ð� ����
	player->lastPongTime =
		std::chrono::system_clock::now();

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : Common_Ping\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port)));

	GameProcessor->Send(player->socket, (FPacketStruct*)& commonPing);
}

void CTCPReceiveProcessor::Common_AnswerId(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Common_AnswerID cCommonAnswerID;
	cCommonAnswerID.Deserialize(recvBuf, &cursor);

	UINT64& steamID = cCommonAnswerID.steamID;
	bool isReconnect = false;

	if (steamID == 0) return;
	// ������ ó�� ///// �̹� �����ϸ鼭 ������ �ٸ� ���(�ܸ��� �ٸ� ���) ��� �Ѵ�.
	{
		CPlayer* beforeUser = GameProcessor->PlayerManager->GetPlayerById(steamID);
		if (beforeUser) { // �̹� �����ϴ� ID
			if (beforeUser->socket != player->socket) { // �ٸ� ����
				player = GameProcessor->AbsorbPlayer(player, beforeUser);
			}
			isReconnect = true;
		}
	}

	CPlayer*& targetUser = player;
	if (!targetUser) return;

	// ����ID ����
	targetUser->steamID = steamID;

	// �ð� ����
	targetUser->lastPongTime = std::chrono::system_clock::now();

	CRoom* currentGameRoom = GameProcessor->RoomManager->GetGameRoom(player);
	// ���� ���� �뿡 �����ӽ� ������ �뺸.
	if (currentGameRoom && isReconnect) {
		currentGameRoom->SendReconnectToAllMember(player);
	}
	// �ƴ϶�� ���ο� �� ���� ����
	else {
		CRoom* room = GameProcessor->RoomManager->GetRoom(targetUser);
		if (room) room->SendRoomInfoToAllMember();
	}

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Common_AnswerId %llu\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), steamID));
}
void CTCPReceiveProcessor::Lobby_InviteFriendRequest(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_InviteFriend_Request cLobbyInviteFriendRequest;
	cLobbyInviteFriendRequest.Deserialize(recvBuf, &cursor);

	UINT64& steamID = cLobbyInviteFriendRequest.targetID;
	if (steamID == 0) return;

	// �÷��̾ ã��, �÷��̾�� ���� �ǻ縦 ���´�.
	CPlayer* targetUser = GameProcessor->PlayerManager->GetPlayerById(steamID);
	if (targetUser) {
		CPlayer* sendPlayer = GameProcessor->PlayerManager->
			GetPlayerBySocket(player->socket);
		if (!sendPlayer) return;
		UINT64 senderId = sendPlayer->steamID;
		// �ʴ����� �̸��� ��� ������.
		FS_Lobby_InviteFriend_Request sLobbyInviteFriendRequest;
		sLobbyInviteFriendRequest.senderID = senderId;

		GameProcessor->Send(targetUser->socket, (FPacketStruct*)&sLobbyInviteFriendRequest);
		WriteLog(ELogLevel::Warning, CLog::Format("C_Lobby_InviteFriend_Request : Invite Send Success.\n"));
	}
	// ���� ���� �ʴ´ٸ�, ������ ����.
	else {
		char msg[] = "ģ���� ã�µ� �����Ͽ����ϴ�.";

		FS_Lobby_InviteFriend_Failed sLobbyInviteFriendFailed;
		memcpy(sLobbyInviteFriendFailed.msg.buf, msg, sizeof(msg));
		sLobbyInviteFriendFailed.msg.len = (int)sizeof(msg);

		GameProcessor->Send(player->socket, (FPacketStruct*)&sLobbyInviteFriendFailed);
		WriteLog(ELogLevel::Warning, CLog::Format("C_Lobby_InviteFriend_Request : Failed to find member.\n"));
	}
	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_InviteFriend_Request %llu\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), steamID));
}

void CTCPReceiveProcessor::Lobby_InviteFriendAnswer(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_InviteFriend_Answer cLobbyInviteFriendAnswer;
	cLobbyInviteFriendAnswer.Deserialize(recvBuf, &cursor);

	bool& isYes = cLobbyInviteFriendAnswer.isAccept;
	UINT64& targetID = cLobbyInviteFriendAnswer.targetID;
	CPlayer* innerPlayer = GameProcessor->PlayerManager->GetPlayerById(targetID);
	if (!innerPlayer) return;

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_InviteFriend_Answer by %llu\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), targetID));

	// �¶����� �ʾҴٸ� �׳� �Ѿ��.
	if (!isYes) return;
	// �¶��ߴٸ�, �� �̵��� �õ��Ѵ�.
	int outSlot;

	// ���̵� ����.
	GameProcessor->RoomManager->MoveRoom(innerPlayer, player, outSlot);
}

void CTCPReceiveProcessor::Lobby_SetPartyKing(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_SetPartyKing cLobbySetPartyKing;
	cLobbySetPartyKing.Deserialize(recvBuf, &cursor);

	// ��Ƽ������ �Ӹ��� ������ �Ľ��Ѵ�.
	int& targetSlot = cLobbySetPartyKing.slot;

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_SetPartyKing by %d\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), targetSlot));

	// ���� ã�´�.
	CRoom* innerRoom = GameProcessor->RoomManager->GetRoom(player);
	if (innerRoom == nullptr ||							// ���� ���ų�
		innerRoom->GetPlayer(0) != player ||	// ��û�ڰ� ������ �ƴϰų�
		targetSlot >= MAX_PLAYER ||						// ������ �߸� �Ǿ��ų�
		targetSlot == 0 ||
		innerRoom->GetPlayer(targetSlot) == nullptr		// ���Կ� �÷��̾ ���ٸ� �����Ѵ�.
		) {
		return;
	}

	// ���� ��ü
	innerRoom->ChangePartyKing(targetSlot);

	// ��Ƽ�� ��ü�� �뺸�Ѵ�.
	innerRoom->SendRoomInfoToAllMember();
}

void CTCPReceiveProcessor::Lobby_FriendKickRequest(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_FriendKick_Request cLobbyFriendKickRequest;
	cLobbyFriendKickRequest.Deserialize(recvBuf, &cursor);

	// ������ ������ �Ľ��Ѵ�.
	int targetSlot = cLobbyFriendKickRequest.slot;

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_FriendKick_Request by %d\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), targetSlot));

	// ���� ã�´�.
	CRoom* innerRoom = GameProcessor->RoomManager->GetRoom(player);
	if (innerRoom == nullptr ||										// ���� ���ų�
		targetSlot < 0 ||
		targetSlot >= MAX_PLAYER ||									// ������ �߸� �Ǿ��ų�
		innerRoom->GetPlayer(targetSlot) == nullptr ||				// ���Կ� �÷��̾ ���ų�
		!(innerRoom->GetPlayer(0) == player ||						// ��û�ڰ� �����̸鼭
			innerRoom->GetPlayer(targetSlot) == player)					// �ڱ� �ڽſ� ���� ó���� �ƴ϶�� ����
		) {

		WriteLog(ELogLevel::Warning, CLog::Format("Failed to kick.\n"));
		return;
	}

	// �÷��̾ ���� �濡�� �߹��Ų����, ���ο� �濡 �־��ش�.
	CPlayer* kickTargetPlayer = innerRoom->GetPlayer(targetSlot);
	GameProcessor->RoomManager->KickPlayer(kickTargetPlayer);
}

void CTCPReceiveProcessor::Lobby_MatchRequest(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_MatchRequest cLobbyMatchRequest;
	cLobbyMatchRequest.Deserialize(recvBuf, &cursor);

	if (player->steamID == 0) return;

	// ��Ŵ����� ��Ī ���� ������ ��û�Ѵ�.
	GameProcessor->RoomManager->ChangeRoomReady(player, cLobbyMatchRequest.isOn);

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_MatchRequest %d id : %llu\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), cLobbyMatchRequest.isOn, player->steamID));
}

void CTCPReceiveProcessor::Debug_RoomStart(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Debug_RoomStart cDebugRoomStart;
	cDebugRoomStart.Deserialize(recvBuf, &cursor);

	CRoom* currentRoom = GameProcessor->RoomManager->GetRoom(player);
	if (!currentRoom) return;

	// ȥ�� �������� ���̵��� �Ѵ�.
	if (currentRoom->GetPlayerCount() != 1) return;
	GameProcessor->RoomManager->ForceJoinRoom(player);

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Debug_RoomStart %llu\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), player->steamID));
}

void CTCPReceiveProcessor::Debug_GameStart(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Debug_GameStart cDebugGameStart;
	cDebugGameStart.Deserialize(recvBuf, &cursor);

	CRoom* currentRoom = GameProcessor->RoomManager->GetRoom(player);

	if (!currentRoom || currentRoom->GetPlayerCount() != 1 || currentRoom->GetState() == GAME) return;

	// ���� ���� �����Ѵٸ� ������ �����Ѵ�.
	bool onSuccess = GameProcessor->RoomManager->ForceJoinGameRoom(player);

	// �������� ���� ���ٸ�, ������� ���ӷ����� ���� �̵��Ѵ�.
	if (!onSuccess) GameProcessor->RoomManager->ForceChangeGameState(currentRoom);

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Debug_GameStart %llu newRoom? : %d\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), player->steamID, !onSuccess));
}

void CTCPReceiveProcessor::InGame_EndGame(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_End_Game cEndGame;
	cEndGame.Deserialize(recvBuf, &cursor);

	// ���� ���̶�� �κ�� ������ �̵���Ų�� ���Ḧ �뺸�Ѵ�.
	CRoom* targetRoom = GameProcessor->RoomManager->GetGameRoom(player);
	if (targetRoom && targetRoom->GetState() == GAME) {
		FS_End_Game sEndGame;

		// Ŀ�ؼ��� �ν�Ʈ�� �÷��̾�� �����Ѵ�.
		for (int i = 0; i < MAX_PLAYER; ++i) {
			auto player = targetRoom->GetPlayer(i);
			if (player && player->state == EGameState::ConnectionLost) {
				GameProcessor->PlayerManager->RemovePlayer(player);
				targetRoom->DeletePlayer(i);
			}
		}

		GameProcessor->RoomManager->ChangeRoomState(targetRoom, EGameState::LOBBY);
		targetRoom->SendToAllMember((FPacketStruct*)&sEndGame);
		targetRoom->SendRoomInfoToAllMember();
	}

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_End_Game\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port)));
}

void CTCPReceiveProcessor::InGame_RPC(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_InGame_RPC cInGameRPC;
	cInGameRPC.Deserialize(recvBuf, &cursor);

	char& type = cInGameRPC.rpcType;
	CRoom* targetRoom = GameProcessor->RoomManager->GetGameRoom(player);
	if (!targetRoom || targetRoom->GetState() != GAME) {
		return;
	}

	switch (type)
	{
		case 0:
		{
			// ��Ƽĳ��Ʈ
			FS_InGame_RPC sInGameRPC;
			sInGameRPC.objectHandle = cInGameRPC.objectHandle;
			sInGameRPC.idenHandle  = cInGameRPC.idenHandle;
			sInGameRPC.argBuf = cInGameRPC.argBuf;

			targetRoom->SendToOtherMember(player->steamID, (FPacketStruct*)&sInGameRPC);
			break;
		}
		case 1:
		{
			// �����Ϳ��� ó��.
			// �۽��ڰ� �����Ͷ�� �����Ѵ�.
			auto masterPlayer = targetRoom->GetPlayer(0);
			if (masterPlayer == player) break;

			// �����Ϳ��� ����
			FS_InGame_RPC sInGameRPC;
			sInGameRPC.objectHandle = cInGameRPC.objectHandle;
			sInGameRPC.idenHandle = cInGameRPC.idenHandle;
			sInGameRPC.argBuf = cInGameRPC.argBuf;

			GameProcessor->Send(masterPlayer->socket, (FPacketStruct*)&sInGameRPC);
			break;
		}
		default:
			WriteLog(ELogLevel::Error, CLog::Format("[%s:%d] : Unknown RPC type.\n", inet_ntoa(player->addr.sin_addr),
				ntohs(player->addr.sin_port)));
			break;
	}

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_InGame_RPC.\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port)));
}

void CTCPReceiveProcessor::InGame_SyncVar(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_InGame_SyncVar cInGameSyncVar;
	cInGameSyncVar.Deserialize(recvBuf, &cursor);

	CRoom* targetRoom = GameProcessor->RoomManager->GetGameRoom(player);
	if (!targetRoom || targetRoom->GetState() != GAME) {
		return;
	}

	// ������ ������ ������ ��Ƽ������ ����.
	FS_InGame_SyncVar sInGameSyncVar;
	sInGameSyncVar.objectHandle = cInGameSyncVar.objectHandle;
	sInGameSyncVar.idenHandle = cInGameSyncVar.idenHandle;
	sInGameSyncVar.argBuf = cInGameSyncVar.argBuf;

	targetRoom->SendToOtherMember(player->steamID, (FPacketStruct*)&sInGameSyncVar);

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_SyncVar.\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port)));
}

void CTCPReceiveProcessor::InGame_ReconnectServer(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Reconnect_Server cReconnectServer;
	cReconnectServer.Deserialize(recvBuf, &cursor);

	if (cReconnectServer.myID == 0) return;
	// ������ ó�� ///// �̹� �����ϸ鼭 ������ �ٸ� ���(�ܸ��� �ٸ� ���) ��� �Ѵ�.
	{
		CPlayer* beforeUser = GameProcessor->PlayerManager->GetPlayerById(cReconnectServer.myID);
		if (beforeUser) { // �̹� �����ϴ� ID
			if (beforeUser->socket != player->socket) { // �ٸ� ����
				player = GameProcessor->AbsorbPlayer(player, beforeUser);
			}
			else {
				return;
			}
		}
	}

	CPlayer*& targetUser = player;
	if (!targetUser) return;

	// ����ID ����
	targetUser->steamID = cReconnectServer.myID;

	// �ð� ����
	targetUser->lastPongTime = std::chrono::system_clock::now();

	GameProcessor->PlayerManager->Print();
	GameProcessor->RoomManager->Print();
	CRoom* room = GameProcessor->RoomManager->GetGameRoom(player);
	if (!room){
		// ���� ������߿� ���� �ִ� ����� �����Ѵ�.
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (cReconnectServer.slot[i] == 0 || cReconnectServer.slot[i] == cReconnectServer.myID) continue;

			CPlayer* roomPlayer = GameProcessor->PlayerManager->GetPlayerById(cReconnectServer.slot[i]);
			if (roomPlayer) room = GameProcessor->RoomManager->GetRoom(roomPlayer);
			if (room) break;
		}
		if (room) {
			// ���� �����Ѵٸ� �� �濡 ����.
			GameProcessor->RoomManager->OutRoom(player);
			player = room->JoinRoom(player);
		}
		else {
			// ���� �������� �ʴ´ٸ� ���� ���� �����.
			GameProcessor->RoomManager->OutRoom(player);
			room = GameProcessor->RoomManager->CreateRoom(player);
			if (cReconnectServer.isGameRoom) GameProcessor->RoomManager->ChangeRoomState(room, EGameState::GAME);
		}
	}

	// ������ �Ϸ�Ǿ����� Ȯ���Ѵ�.
	int playerCount = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (cReconnectServer.slot[i] != 0) ++playerCount;
	}
	if (room->GetPlayerCount() == playerCount) {
		// ������ �Ϸ� �Ǿ��ٸ� ������ �ٽ� ���߰� �ٽ� �÷����Ѵ�.
		for (int i = 1; i < MAX_PLAYER; ++i) {
			// ������ ã�´�.
			auto innerRoomPlayer = room->GetPlayer(i);
			if (innerRoomPlayer && cReconnectServer.slot[0] == innerRoomPlayer->steamID) {
				room->SwapPlayer(0, i);
				break;
			}
		}
		room->SendRoomInfoToAllMember();
	}
	GameProcessor->PlayerManager->Print();
	GameProcessor->RoomManager->Print();

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Reconnect_Server %llu\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), cReconnectServer.myID));
}