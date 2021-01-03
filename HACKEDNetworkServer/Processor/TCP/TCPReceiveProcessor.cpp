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
	// 플레이어가 나갔다.
	if (receiveLen == 0) {
		WriteLog(ELogLevel::Error, CLog::Format("ReceiveData : CloseConnection."));
		return false;
	}
	if (receiveLen < 0 || receiveLen > PACKET_BUFSIZE) {
		WriteLog(ELogLevel::Error, CLog::Format("ReceiveData : Wrong ReceiveLen"));
		return false;
	}

	// 딜레이된 버퍼가 있는지 확인하고 있다면 꺼내서 현재 버퍼에 합쳐준다.
	if (player->delayedTCPDataLen > 0) {
		player->PopDelayData(recvBuf, receiveLen, true);
	}

	int cursor = 0, bufLen = 0;
	FPacketStruct packetStruct;
	while (cursor < receiveLen) {
		// ENUM을 제외한 길이를 계산한다.
		bufLen = IntDeserialize(recvBuf, &cursor);
		if (bufLen < 0) {
			WriteLog(ELogLevel::Error, CLog::Format("TCP Receive buflen < 0, bufSize = %d, cursor = %d, len = %d", bufLen, cursor, receiveLen));
			return false;
		}
		if (bufLen > PACKET_BUFSIZE) {
			WriteLog(ELogLevel::Error, CLog::Format("TCP Receive buflen > BUFSIZE, bufSize = %d, cursor = %d, len = %d", bufLen, cursor, receiveLen));
			return false;
		}

		// 수신량을 넘어서 데이터가 넘어온다면 남은 데이터를 저장하고 리턴한다.
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

	// 시간 갱신
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
	// 재접속 처리 ///// 이미 존재하면서 소켓이 다를 경우(단말이 다를 경우) 흡수 한다.
	{
		CPlayer* beforeUser = GameProcessor->PlayerManager->GetPlayerById(steamID);
		if (beforeUser) { // 이미 존재하는 ID
			if (beforeUser->socket != player->socket) { // 다른 소켓
				player = GameProcessor->AbsorbPlayer(player, beforeUser);
			}
			isReconnect = true;
		}
	}

	CPlayer*& targetUser = player;
	if (!targetUser) return;

	// 스팀ID 갱신
	targetUser->steamID = steamID;

	// 시간 갱신
	targetUser->lastPongTime = std::chrono::system_clock::now();

	CRoom* currentGameRoom = GameProcessor->RoomManager->GetGameRoom(player);
	// 게임 중인 룸에 재접속시 재접속 통보.
	if (currentGameRoom && isReconnect) {
		currentGameRoom->SendReconnectToAllMember(player);
	}
	// 아니라면 새로운 방 정보 전송
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

	// 플레이어를 찾아, 플레이어에게 수락 의사를 묻는다.
	CPlayer* targetUser = GameProcessor->PlayerManager->GetPlayerById(steamID);
	if (targetUser) {
		CPlayer* sendPlayer = GameProcessor->PlayerManager->
			GetPlayerBySocket(player->socket);
		if (!sendPlayer) return;
		UINT64 senderId = sendPlayer->steamID;
		// 초대자의 이름을 담아 보낸다.
		FS_Lobby_InviteFriend_Request sLobbyInviteFriendRequest;
		sLobbyInviteFriendRequest.senderID = senderId;

		GameProcessor->Send(targetUser->socket, (FPacketStruct*)&sLobbyInviteFriendRequest);
		WriteLog(ELogLevel::Warning, CLog::Format("C_Lobby_InviteFriend_Request : Invite Send Success.\n"));
	}
	// 존재 하지 않는다면, 에러를 날림.
	else {
		char msg[] = "친구를 찾는데 실패하였습니다.";

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

	// 승락하지 않았다면 그냥 넘어간다.
	if (!isYes) return;
	// 승락했다면, 룸 이동을 시도한다.
	int outSlot;

	// 룸이동 수행.
	GameProcessor->RoomManager->MoveRoom(innerPlayer, player, outSlot);
}

void CTCPReceiveProcessor::Lobby_SetPartyKing(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_SetPartyKing cLobbySetPartyKing;
	cLobbySetPartyKing.Deserialize(recvBuf, &cursor);

	// 파티장으로 임명할 슬롯을 파싱한다.
	int& targetSlot = cLobbySetPartyKing.slot;

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_SetPartyKing by %d\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), targetSlot));

	// 방을 찾는다.
	CRoom* innerRoom = GameProcessor->RoomManager->GetRoom(player);
	if (innerRoom == nullptr ||							// 방이 없거나
		innerRoom->GetPlayer(0) != player ||	// 요청자가 방장이 아니거나
		targetSlot >= MAX_PLAYER ||						// 슬롯이 잘못 되었거나
		targetSlot == 0 ||
		innerRoom->GetPlayer(targetSlot) == nullptr		// 슬롯에 플레이어가 없다면 무시한다.
		) {
		return;
	}

	// 슬롯 교체
	innerRoom->ChangePartyKing(targetSlot);

	// 파티장 교체를 통보한다.
	innerRoom->SendRoomInfoToAllMember();
}

void CTCPReceiveProcessor::Lobby_FriendKickRequest(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_FriendKick_Request cLobbyFriendKickRequest;
	cLobbyFriendKickRequest.Deserialize(recvBuf, &cursor);

	// 강퇴할 슬롯을 파싱한다.
	int targetSlot = cLobbyFriendKickRequest.slot;

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Lobby_FriendKick_Request by %d\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), targetSlot));

	// 방을 찾는다.
	CRoom* innerRoom = GameProcessor->RoomManager->GetRoom(player);
	if (innerRoom == nullptr ||										// 방이 없거나
		targetSlot < 0 ||
		targetSlot >= MAX_PLAYER ||									// 슬롯이 잘못 되었거나
		innerRoom->GetPlayer(targetSlot) == nullptr ||				// 슬롯에 플레이어가 없거나
		!(innerRoom->GetPlayer(0) == player ||						// 요청자가 방장이면서
			innerRoom->GetPlayer(targetSlot) == player)					// 자기 자신에 대한 처리가 아니라면 무시
		) {

		WriteLog(ELogLevel::Warning, CLog::Format("Failed to kick.\n"));
		return;
	}

	// 플레이어를 기존 방에서 추방시킨다음, 새로운 방에 넣어준다.
	CPlayer* kickTargetPlayer = innerRoom->GetPlayer(targetSlot);
	GameProcessor->RoomManager->KickPlayer(kickTargetPlayer);
}

void CTCPReceiveProcessor::Lobby_MatchRequest(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_Lobby_MatchRequest cLobbyMatchRequest;
	cLobbyMatchRequest.Deserialize(recvBuf, &cursor);

	if (player->steamID == 0) return;

	// 룸매니저에 매칭 상태 변경을 요청한다.
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

	// 혼자 있을때만 방이동을 한다.
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

	// 게임 룸이 존재한다면 강제로 참여한다.
	bool onSuccess = GameProcessor->RoomManager->ForceJoinGameRoom(player);

	// 게임중인 룸이 없다면, 현재룸을 게임룸으로 강제 이동한다.
	if (!onSuccess) GameProcessor->RoomManager->ForceChangeGameState(currentRoom);

	WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : C_Debug_GameStart %llu newRoom? : %d\n", inet_ntoa(player->addr.sin_addr),
		ntohs(player->addr.sin_port), player->steamID, !onSuccess));
}

void CTCPReceiveProcessor::InGame_EndGame(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_End_Game cEndGame;
	cEndGame.Deserialize(recvBuf, &cursor);

	// 게임 중이라면 로비로 스탯을 이동시킨뒤 종료를 통보한다.
	CRoom* targetRoom = GameProcessor->RoomManager->GetGameRoom(player);
	if (targetRoom && targetRoom->GetState() == GAME) {
		FS_End_Game sEndGame;

		// 커넥션이 로스트된 플레이어는 제거한다.
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
			// 멀티캐스트
			FS_InGame_RPC sInGameRPC;
			sInGameRPC.objectHandle = cInGameRPC.objectHandle;
			sInGameRPC.idenHandle  = cInGameRPC.idenHandle;
			sInGameRPC.argBuf = cInGameRPC.argBuf;

			targetRoom->SendToOtherMember(player->steamID, (FPacketStruct*)&sInGameRPC);
			break;
		}
		case 1:
		{
			// 마스터에서 처리.
			// 송신자가 마스터라면 무시한다.
			auto masterPlayer = targetRoom->GetPlayer(0);
			if (masterPlayer == player) break;

			// 마스터에게 전달
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

	// 본인을 제외한 나머지 파티원에게 전달.
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
	// 재접속 처리 ///// 이미 존재하면서 소켓이 다를 경우(단말이 다를 경우) 흡수 한다.
	{
		CPlayer* beforeUser = GameProcessor->PlayerManager->GetPlayerById(cReconnectServer.myID);
		if (beforeUser) { // 이미 존재하는 ID
			if (beforeUser->socket != player->socket) { // 다른 소켓
				player = GameProcessor->AbsorbPlayer(player, beforeUser);
			}
			else {
				return;
			}
		}
	}

	CPlayer*& targetUser = player;
	if (!targetUser) return;

	// 스팀ID 갱신
	targetUser->steamID = cReconnectServer.myID;

	// 시간 갱신
	targetUser->lastPongTime = std::chrono::system_clock::now();

	GameProcessor->PlayerManager->Print();
	GameProcessor->RoomManager->Print();
	CRoom* room = GameProcessor->RoomManager->GetGameRoom(player);
	if (!room){
		// 기존 멤버들중에 룸이 있는 멤버에 가야한다.
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (cReconnectServer.slot[i] == 0 || cReconnectServer.slot[i] == cReconnectServer.myID) continue;

			CPlayer* roomPlayer = GameProcessor->PlayerManager->GetPlayerById(cReconnectServer.slot[i]);
			if (roomPlayer) room = GameProcessor->RoomManager->GetRoom(roomPlayer);
			if (room) break;
		}
		if (room) {
			// 룸이 존재한다면 그 방에 들어간다.
			GameProcessor->RoomManager->OutRoom(player);
			player = room->JoinRoom(player);
		}
		else {
			// 룸이 존재하지 않는다면 방을 새로 만든다.
			GameProcessor->RoomManager->OutRoom(player);
			room = GameProcessor->RoomManager->CreateRoom(player);
			if (cReconnectServer.isGameRoom) GameProcessor->RoomManager->ChangeRoomState(room, EGameState::GAME);
		}
	}

	// 복구가 완료되었는지 확인한다.
	int playerCount = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (cReconnectServer.slot[i] != 0) ++playerCount;
	}
	if (room->GetPlayerCount() == playerCount) {
		// 복구가 완료 되었다면 방장을 다시 맞추고 다시 플레이한다.
		for (int i = 1; i < MAX_PLAYER; ++i) {
			// 방장을 찾는다.
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