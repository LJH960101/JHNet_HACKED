#pragma once
/*
*	패킷 전송에 사용되는 구조체들입니다.
*/

#include "PacketStructMacro.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/NetworkInfo.h"
#include "JHNET.h"

struct FPacketStruct {
	EMessageType type;

	virtual size_t GetSize() {
		return sizeof(type);
	}
	virtual size_t Serialize(char* buf) {
		return JHNETSerializer::TSerialize(buf, type);
	}
	virtual void Deserialize(char* buf, int* cursor) {
		type = JHNETSerializer::TDeserialize<EMessageType>(buf, cursor);
	}
};

struct F_Common_Echo : FPacketStruct {
	F_Common_Echo() {
		type = EMessageType::Common_Echo;
	}
	FSerializableBuf newString;

	PACK_STRUCT(newString);
};

struct F_Common_Ping : FPacketStruct {
	F_Common_Ping() {
		type = EMessageType::Common_Ping;
	}
};

struct FS_Common_RequestID : FPacketStruct {
	FS_Common_RequestID() {
		type = EMessageType::S_Common_RequestId;
	}
};

struct FC_Common_AnswerID : FPacketStruct {
	FC_Common_AnswerID() {
		type = EMessageType::C_Common_AnswerId;
	}
	UINT64 steamID;

	PACK_STRUCT(steamID);
};

struct FC_Lobby_InviteFriend_Request : FPacketStruct {
	FC_Lobby_InviteFriend_Request() {
		type = EMessageType::C_Lobby_InviteFriend_Request;
	}
	UINT64 targetID;

	PACK_STRUCT(targetID);
};

struct FS_Lobby_InviteFriend_Request : FPacketStruct {
	FS_Lobby_InviteFriend_Request() {
		type = EMessageType::S_Lobby_InviteFriend_Request;
	}
	UINT64 senderID;

	PACK_STRUCT(senderID);
};

struct FC_Lobby_InviteFriend_Answer : FPacketStruct {
	FC_Lobby_InviteFriend_Answer() {
		type = EMessageType::C_Lobby_InviteFriend_Answer;
	}
	bool isAccept;
	UINT64 targetID;

	PACK_STRUCT(isAccept, targetID);
};

struct FS_Room_Info : FPacketStruct {
	FS_Room_Info() {
		type = EMessageType::S_Room_Info;
	}
	UINT64 slot[MAX_PLAYER];

	PACK_STRUCT(slot);
};

struct FS_Room_ChangeState : FPacketStruct {
	FS_Room_ChangeState() {
		type = EMessageType::S_Room_ChangeState;
	}
	int slot;
	UINT64 steamID;

	PACK_STRUCT(slot, steamID);
};

struct FS_Lobby_InviteFriend_Failed : FPacketStruct {
	FS_Lobby_InviteFriend_Failed() {
		type = EMessageType::S_Lobby_InviteFriend_Failed;
	}
	FSerializableBuf msg;

	PACK_STRUCT(msg);
};

struct FC_Lobby_SetPartyKing : FPacketStruct {
	FC_Lobby_SetPartyKing() {
		type = EMessageType::C_Lobby_SetPartyKing;
	}
	int slot;

	PACK_STRUCT(slot);
};

struct FC_Lobby_FriendKick_Request : FPacketStruct {
	FC_Lobby_FriendKick_Request() {
		type = EMessageType::C_Lobby_FriendKick_Request;
	}
	int slot;

	PACK_STRUCT(slot);
};

struct FC_Lobby_MatchRequest : FPacketStruct {
	FC_Lobby_MatchRequest() {
		type = EMessageType::C_Lobby_MatchRequest;
	}
	bool isOn;

	PACK_STRUCT(isOn);
};

struct FS_Lobby_MatchAnswer : FPacketStruct {
	FS_Lobby_MatchAnswer() {
		type = EMessageType::S_Lobby_MatchAnswer;
	}
	bool isOn;
	int slot;

	PACK_STRUCT(isOn, slot);
};

struct FS_Lobby_GameStart : FPacketStruct {
	FS_Lobby_GameStart() {
		type = EMessageType::S_Lobby_GameStart;
	}
};

struct FC_Debug_RoomStart : FPacketStruct {
	FC_Debug_RoomStart() {
		type = EMessageType::C_Debug_RoomStart;
	}
};

struct FC_Debug_GameStart : FPacketStruct {
	FC_Debug_GameStart() {
		type = EMessageType::C_Debug_GameStart;
	}
};

struct FC_InGame_RPC : FPacketStruct {
	FC_InGame_RPC() {
		type = EMessageType::C_InGame_RPC;
	}
	char rpcType;
	FSerializableBuf objectHandle;
	FSerializableBuf idenHandle;
	FSerializableBuf argBuf;

	PACK_STRUCT(rpcType, objectHandle, idenHandle, argBuf);
};

struct FS_InGame_RPC : FPacketStruct {
	FS_InGame_RPC() {
		type = EMessageType::S_InGame_RPC;
	}
	FSerializableBuf objectHandle;
	FSerializableBuf idenHandle;
	FSerializableBuf argBuf;

	PACK_STRUCT(objectHandle, idenHandle, argBuf);
};

struct FC_InGame_SyncVar : FPacketStruct {
	FC_InGame_SyncVar() {
		type = EMessageType::C_InGame_SyncVar;
	}
	FSerializableBuf objectHandle;
	FSerializableBuf idenHandle;
	FSerializableBuf argBuf;

	PACK_STRUCT(objectHandle, idenHandle, argBuf);
};

struct FS_InGame_SyncVar : FPacketStruct {
	FS_InGame_SyncVar() {
		type = EMessageType::S_InGame_SyncVar;
	}
	FSerializableBuf objectHandle;
	FSerializableBuf idenHandle;
	FSerializableBuf argBuf;

	PACK_STRUCT(objectHandle, idenHandle, argBuf);
};

struct FC_UDP_Reg : FPacketStruct {
	FC_UDP_Reg() {
		type = EMessageType::C_UDP_Reg;
	}
	UINT64 steamID;

	PACK_STRUCT(steamID);
};

struct FS_UDP_TCPReconnectRequest : FPacketStruct {
	FS_UDP_TCPReconnectRequest() {
		type = EMessageType::S_UDP_TCPReconnectRequest;
	}
};

struct FS_UDP_UDPReconnectRequest : FPacketStruct {
	FS_UDP_UDPReconnectRequest() {
		type = EMessageType::S_UDP_UDPReconnectRequest;
	}
};

struct F_UDP_Heartbeat : FPacketStruct {
	F_UDP_Heartbeat() {
		type = EMessageType::UDP_HeartBeat;
	}
};

struct FS_UDP_Response : FPacketStruct {
	FS_UDP_Response() {
		type = EMessageType::S_UDP_Response;
	}
};

struct FS_Disconnect_Slot : FPacketStruct {
	FS_Disconnect_Slot() {
		type = EMessageType::S_Disconnect_Slot;
	}
	int slot;

	PACK_STRUCT(slot);
};

struct FS_Reconnect_Slot : FPacketStruct {
	FS_Reconnect_Slot() {
		type = EMessageType::S_Reconnect_Slot;
	}
	int slot;

	PACK_STRUCT(slot);
};

struct FS_Reconnect : FPacketStruct {
	FS_Reconnect() {
		type = EMessageType::S_Reconnect;
	}
	UINT64 slot[MAX_PLAYER];

	PACK_STRUCT(slot);
};

struct FC_End_Game : FPacketStruct {
	FC_End_Game() {
		type = EMessageType::C_End_Game;
	}
};

struct FS_End_Game : FPacketStruct {
	FS_End_Game() {
		type = EMessageType::S_End_Game;
	}
};

struct FC_Reconnect_Server : FPacketStruct {
	FC_Reconnect_Server() {
		type = EMessageType::C_Reconnect_Server;
	}
	UINT64 slot[MAX_PLAYER];
	bool isGameRoom;
	UINT64 myID;

	PACK_STRUCT(slot, isGameRoom, myID);
};