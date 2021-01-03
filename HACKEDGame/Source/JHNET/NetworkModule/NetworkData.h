#pragma once
#define MAX_STRING_BUF 100
#include <iostream>
#include "JHNET.h"

/*
	Used in network.
	[Type(Sever,Client, Common)]_[Place to use]_[Function]
*/
enum class EMessageType {
	// COMMON -----
	// 단순 에코
	Common_Echo,
	// 핑
	Common_Ping,

	// UDP 등록 및 핸드세이킹
	C_UDP_Reg,
	// TCP 재등록을 요청
	S_UDP_TCPReconnectRequest,
	// UDP 재등록을 요청
	S_UDP_UDPReconnectRequest,
	// UDP 응답
	S_UDP_Response,

	// 스팀 ID 요청
	S_Common_RequestId,
	C_Common_AnswerId,
	// 디버그 전용 메세지
	// 대기룸 시작
	C_Debug_RoomStart,
	// 게임룸 강제 시작
	C_Debug_GameStart,

	// 서버에서 룸 정보 전송
	S_Room_Info,
	// 서버에서 룸 변경 내용 전송
	S_Room_ChangeState,

	// MATCH -----
	// 클라에서 친구 초대 요청
	C_Lobby_InviteFriend_Request,
	// 서버에서 친구 초대 승락 요청
	S_Lobby_InviteFriend_Request,
	// 클라에서 친구 초대 요청 응답
	C_Lobby_InviteFriend_Answer,
	// 서버에서 친구 초대 실패 응답
	S_Lobby_InviteFriend_Failed,
	// 클라에서 파티장 설정
	C_Lobby_SetPartyKing,
	// 클라에서 친구 강퇴 요청
	C_Lobby_FriendKick_Request,

	// 클라에서 매칭 시작 요청 
	C_Lobby_MatchRequest,
	// 서버에서 매칭 시작 응답
	S_Lobby_MatchAnswer,
	// 서버에서 매칭 성공 통보
	S_Lobby_GameStart,

	// INGAME
	// RPC
	C_InGame_RPC,
	S_InGame_RPC,

	// RPC
	C_InGame_SyncVar,
	S_InGame_SyncVar,

	// Disconnect
	S_Disconnect_Slot,
	S_Reconnect_Slot,
	S_Reconnect,

	S_End_Game,
	C_End_Game,

	C_Reconnect_Server,

	UDP_HeartBeat = 100,

	MAX
};

struct FSerializableBuf
{
	FSerializableBuf& operator = (const FSerializableBuf& b) {
		len = b.len;
		memcpy(buf, b.buf, b.len);
		return *this;
	}
	int len;
	char buf[MAX_STRING_BUF];
	inline size_t GetSize() {
		return len + sizeof(int);
	}
};