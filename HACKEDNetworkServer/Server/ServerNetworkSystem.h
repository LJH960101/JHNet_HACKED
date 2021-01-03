#pragma once
/*
	TCP/UDP 를 가동하는 싱글톤 매니저 클래스
*/

#include "JHNETGameServer.h"
#include "Server.h"
#include "Processor/DataType/ObjectPool.h"
#include "Processor/DataType/Socket.h"
#include <string>
#include <tuple>
#include <map>
#include <thread>

class CUDPProcessor;
class CSocket;

#define NONE_STEAM_PING_DELAY 1
#define PING_DELAY 3
#define PING_FINAL_DELAY 60
#define TCP_PROCESS_WAIT_MILLSEC 1000

enum class ELogLevel;

class CServerNetworkSystem
{
public:
	static CServerNetworkSystem* GetInstance();
	~CServerNetworkSystem();
	bool Run();
	void Shutdown();
	void Send(CSocket* socket, const char* buf, const int& sendLen);
	void SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen);
	void Close(CSocket* socket);
	bool IsRun();

	void OnAccept(CSocket* socket, const FSocketAddrIn& clientAddr);
	void OnRecvTCP(CSocket* socket, char* buf, const int& len);
	void OnRecvUDP(const FSocketAddrIn& udpAddr, char* buf, const int& len);
	void OnClose(CSocket* socket);

	void PrintObjectPools();
	void Print();
	void PrintSummary();

	CObjectPool<CSocket>* socketObjectPool;

private:
	static CServerNetworkSystem* _instance;

	CServerNetworkSystem();
	void WriteLog(const ELogLevel& level, const std::string& msg);

	// IOCP or EPOLL
	IServer* _server;
	class CGameProcessor* GameProcessor;
};