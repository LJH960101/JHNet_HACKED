#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#include "UDPReceiveProcessor.h"
#include "Server/ServerNetworkSystem.h"
#include "Processor/Manager/PlayerManager.h"
#include "Processor/Manager/RoomManager.h"
#include "Processor/DataType/Room.h"
#include "Processor/DataType/Player.h"
#include "Processor/GameProcessor.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/PacketStruct.h"
#include "NetworkModule/Log.h"
#include "NetworkModule/JHNETTool.h"
#include <stdio.h>
#include <chrono>
#include <ctime> 

using namespace JHNETTool;
using namespace std;
using namespace JHNETSerializer;

bool CUDPReceiveProcessor::ReceiveData(const FSocketAddrIn& addr, char* buf, int receiveLen)
{
	if (receiveLen < 0 || receiveLen > PACKET_BUFSIZE) return false;
	if (!ServerNetworkSystem || !GameProcessor || !GameProcessor->PlayerManager || !GameProcessor->RoomManager) {
		std::cout << "[CUDPReceiveProcessor::ReceiveData] Critical Error!!!!\n";
		return false;
	}

	CPlayer* player = GameProcessor->PlayerManager->GetPlayerByUDPAddr(addr);

	// 남은 버퍼가 있는지 확인하고 있다면 꺼내서 현재 버퍼에 합쳐준다.
	if (player && player->delayedUDPData > 0) {
		player->PopDelayData(buf, receiveLen, false);
	}

	int cursor = 0, bufLen = 0;
	FPacketStruct packetStruct;
	char* recvBuf = buf;
	while (cursor < receiveLen) {
		// 데이터 처리
		bufLen = IntDeserialize(recvBuf, &cursor);
		if (bufLen < 0) {
			WriteLog(ELogLevel::Error, CLog::Format("UDP Receive buflen < 0, bufSize = %d, cursor = %d, len = %d", bufLen, cursor, receiveLen));
			return false;
		}
		if (bufLen > PACKET_BUFSIZE) {
			WriteLog(ELogLevel::Error, CLog::Format("UDP Receive buflen > BUFSIZE, bufSize = %d, cursor = %d, len = %d", bufLen, cursor, receiveLen));
			return false;
		}

		// 수신량을 넘어서 데이터가 넘어온다면 남은 데이터를 저장하고 리턴한다.
		if (cursor + bufLen > receiveLen) {
			if (!player) return true;
			cursor -= (int)sizeof(INT32);
			player->PushDelayData(recvBuf, cursor, receiveLen, false);
			return true;
		}

		int currentCursor = cursor;
		packetStruct.Deserialize(recvBuf, &currentCursor);
		switch (packetStruct.type)
		{
		case EMessageType::UDP_HeartBeat:
		{
			HeatBeat(player, recvBuf, cursor, addr);
			break;
		}
		case EMessageType::C_UDP_Reg:
		{
			UDPReg(player, recvBuf, cursor, addr);
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
		default:
		{
			std::string logString = CLog::Format("Unknown receive type!!!! : %d", (int)packetStruct.type);
			WriteLog(ELogLevel::Error, logString);
			cursor += bufLen;
			break;
		}
		}
	}

	/*if (!player) {
		char* buf = ServerNetworkSystem->bufObjectPool->PopObject();
		FS_UDP_UDPReconnectRequest sUDPUDPReconnectRequest;
		sUDPUDPReconnectRequest.Serialize(buf);

		GameProcessor->SendTo(const_cast<FSocketAddrIn*>(&addr), buf, (int)sUDPUDPReconnectRequest.GetSize());
		ServerNetworkSystem->bufObjectPool->ReturnObject(buf);
	}*/
	return true;
}

void CUDPReceiveProcessor::SetGameProcessor(class CGameProcessor* gameProcessor)
{
	GameProcessor = gameProcessor;
}

void CUDPReceiveProcessor::WriteLog(ELogLevel level, std::string msg)
{
	CLog::WriteLog(ELogType::UDPReceiveProcessor, level, msg);
}

CUDPReceiveProcessor::CUDPReceiveProcessor()
{
	ServerNetworkSystem = CServerNetworkSystem::GetInstance();
}

void CUDPReceiveProcessor::HeatBeat(CPlayer*& player, char* recvBuf, int& cursor, const FSocketAddrIn& clientAddr)
{
	F_UDP_Heartbeat heartBeat;
	heartBeat.Deserialize(recvBuf, &cursor);

	GameProcessor->SendTo(const_cast<FSocketAddrIn&>(clientAddr), (FPacketStruct*)&heartBeat);
}

void CUDPReceiveProcessor::UDPReg(CPlayer*& player, char* recvBuf, int& cursor, const FSocketAddrIn& clientAddr)
{
	FC_UDP_Reg cUDPReg;
	cUDPReg.Deserialize(recvBuf, &cursor);

	UINT64& steamId = cUDPReg.steamID;

	if (steamId == 0) return;

	// 이미 연결되었다면? 리턴.
	if (player) return;

	WriteLog(ELogLevel::Warning, CLog::Format(" [STEAM: %llu ] C_UDP_Reg  : try to reg\n", steamId));

	// UDP Addr 변경중에 플레이어 정보가 사라지면 다시해야하므로 무한 루프를 건다.
	while (true) {
		// 해당 TCPAddr에 UDP에 연결되지 않은 유저를 찾는다.
		player = GameProcessor->PlayerManager->GetNoUDPPlayerById(steamId);
		if (!player) player = GameProcessor->PlayerManager->GetPlayerById(steamId);
		if (!player) {
			WriteLog(ELogLevel::Error, CLog::Format("[ STEAM: %llu ] C_UDP_Reg  : not exist in tcp player...\n", steamId));

			FS_UDP_TCPReconnectRequest sUDP_TCPReconnectRequest;

			GameProcessor->SendTo(clientAddr, (FPacketStruct*)& sUDP_TCPReconnectRequest);
			return;
		}

		FSocketAddrIn clientSockAddr = FSocketAddrIn(clientAddr);

		// 변경하는데 성공?
		if (player->SetUDPAddr(clientSockAddr)) {
			FS_UDP_Response sUDPResponse;

			GameProcessor->SendTo(player->udpAddr, (FPacketStruct*)& sUDPResponse);
			WriteLog(ELogLevel::Warning, CLog::Format("[ STEAM: %llu ] C_UDP_Reg  : SUCESS!\n", steamId));
			return;
		}
	}
}

void CUDPReceiveProcessor::InGame_RPC(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_InGame_RPC cInGameRPC;
	cInGameRPC.Deserialize(recvBuf, &cursor);

	char& type = cInGameRPC.rpcType;

	// Get Player's Socket Info.
	if (!player) {
		WriteLog(ELogLevel::Error, CLog::Format("C_InGame_RPC  : not exist player...\n"));
		return;
	}
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
			sInGameRPC.idenHandle = cInGameRPC.idenHandle;
			sInGameRPC.argBuf = cInGameRPC.argBuf;

			targetRoom->SendToOtherMember(player->steamID, (FPacketStruct*)&sInGameRPC, 0, false);
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

			if (!masterPlayer->IsHaveUDP())
				GameProcessor->Send(masterPlayer->socket, (FPacketStruct*)&sInGameRPC);
			else 
				GameProcessor->SendTo(masterPlayer->udpAddr, (FPacketStruct*)&sInGameRPC);
			break;
		}
		default:
			WriteLog(ELogLevel::Error, CLog::Format("[%s:%d] : Unknown RPC type.\n", inet_ntoa(player->addr.sin_addr),
				ntohs(player->addr.sin_port)));
			break;
	}
}

void CUDPReceiveProcessor::InGame_SyncVar(CPlayer*& player, char* recvBuf, int& cursor)
{
	FC_InGame_SyncVar cInGameSyncVar;
	cInGameSyncVar.Deserialize(recvBuf, &cursor);

	// Get Player's Socket Info.
	if (!player) {
		WriteLog(ELogLevel::Error, CLog::Format("C_InGame_SyncVar  : not exist player...\n"));
		return;
	}
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
}
