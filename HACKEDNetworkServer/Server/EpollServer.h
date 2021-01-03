#pragma once
#ifdef __linux__
#include "JHNETGameServer.h"
#include "Server.h"
#include "Processor/DataType/Player.h"
#include "Processor/DataType/ObjectPool.h"
#include "Processor/DataType/Socket.h"
#include <string>
#include <tuple>
#include <thread>
#include <map>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CONNECTION 500
#define MAX_EVENT 500
#define SOCKET_ERROR -1

enum ELogLevel;
class CRoomManager;
class CPlayerManager;
class CServerNetworkSystem;

using std::map;
using std::thread;

struct FBuf
{
	char* buf;
	FBuf() {
		buf = new char[PACKET_BUFSIZE];
	}
	~FBuf() {
		delete[] buf;
	}
};

class CEpollServer final : public IServer
{
public:
	static CEpollServer* GetInstance();
	~CEpollServer();

	// Interface of IServer
	virtual bool Run() override;
	virtual void Send(CPlayer* player, const char* buf, const int& sendLen, bool isTCP) override;
	virtual void Close(CSocket* socket) override;
	virtual void SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen) override;
	virtual bool IsRun() override;
	virtual void Shutdown() override;
	virtual void PrintObjectPools() override;

private:

	static CEpollServer* _instance;
	CEpollServer();
	bool _bIsRun;

	CRoomManager* RoomManager;
	CPlayerManager* PlayerManager;
	CServerNetworkSystem* ServerNetworkSystem;

	CObjectPool<FBuf>* _bufObjectPool;

	// Epoll
	int _epollFd;
	epoll_event* _epollEvents;
	epoll_event _epollEvent;

	// TCP
	SOCKET _tcpListenSocket;
	SOCKET _tcpClientSocket;
	FSocketAddrIn _tcpAddrIn;
	int _tcpAcceptLen;
	socklen_t _tcpAdrSize;

	// UDP
	SOCKET _udpSocket;
	FSocketAddrIn _udpAddrIn;
	socklen_t _udpAdrSize;

	// Event Process
	void _OnEvent(const epoll_event& newEvent);
	void _ErrorProc(const epoll_event& newEvent);
	void _TCPRecvProc(const epoll_event& newEvent);
	void _UDPRecvProc(const epoll_event& newEvent);
	void _AcceptProc(const epoll_event& newEvent);

	void _CloseSocket(SOCKET socket);

	// Worker
	std::thread* _workerThread;
	static void _WorkerThread(CEpollServer* owner);

	// Init
	bool _InitEpoll();
	bool _InitTCPListenSocket();
	bool _InitUDPSocket();
	bool _InitWorkerThread();

	void WriteLog(const ELogLevel& level, const std::string& msg);

	// 소켓에 논블로킹 소켓 플래그를 씌워준다.
	int _SetNonBlockingSocket(const SOCKET& socket);
	SOCKET _CreateSocket(const char* port, bool isTCP = false);

	// 중복 close 처리를 막기위해 소켓마다 ON OFF 처리를 하는 map.
	map<CSocket*, bool> sockStates;
};
#endif