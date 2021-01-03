#pragma once
#ifdef _WIN32
/*
	Windows IOCP를 사용하여 Accept Close Send Recv를 클래스.
*/

#include "JHNETGameServer.h"
#include "Server.h"
#include "Processor/DataType/Overlapped.h"
#include "Processor/DataType/ObjectPool.h"
#include "Processor/DataType/Socket.h"
#include <WinSock2.h>
#include <string>
#include <tuple>
#include <thread>
#include <map>

enum class ELogLevel;
class CServerNetworkSystem;

using std::map;
using std::thread;

class CIOCPServer final : public IServer
{
public:
	static CIOCPServer* GetInstance();
	~CIOCPServer();

	// Interface of IServer
	virtual bool Run() override;
	virtual bool IsRun() override;
	virtual void Send(CSocket* socket, const char* buf, const int& sendLen) override;
	virtual void Close(CSocket* socket) override;
	virtual void SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen) override;
	virtual void Shutdown() override;
	virtual void PrintObjectPools() override;

	HANDLE hcp;

private:
	DWORD recvBytes;
	DWORD flags = 0;
	int UDPAddrLen = sizeof(FSocketAddrIn);

	void PostSend(CSocket* socket, const char* buf, const int& sendLen);
	void PostUDPSend(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen);
	void PostClose(CSocket* socket);
	void CloseSocket(CSocket* socket);
	void PostAccept();
	void PostRecv(FBuffuerableOverlapped* overlap, bool isTCP);

	static CIOCPServer* _instance;
	CIOCPServer();
	bool _bIsRun;

	CServerNetworkSystem* ServerNetworkSystem;

	// TCP
	CSocket* _tcpListenSocket;
	DWORD _tcpAcceptLen;

	// UDP
	SOCKET _udpRecvSocket;
	FSocketAddrIn _udpServeraddr;

	void WriteLog(const ELogLevel& level, const std::string& msg);

	static DWORD WINAPI WorkerThread(LPVOID arg);

	// overlapped 타입에 대한 처리 : TCP
	void _AcceptProc(FAcceptOverlapped* overlap);
	void _RecvProc(FBuffuerableOverlapped* overlap, const int& len, bool isTCP);
	void _SendProc(FSendOverlapped* overlap);
	void _CloseProc(FCloseOverlapped* overlap);

	// 초기화 관련 함수
	bool InitWorkerThread();
	bool InitTCPListenSocket();
	bool InitUDPListenSocket();
	bool InitIOCP();
	bool InitObjectPools();

	static FSocketAddrIn* ADDRToADDRIN(FSocketAddr* addr);

	// Object Pools
	CObjectPool<FAcceptOverlapped>* _acceptOverlapObjectPool = nullptr;
	CObjectPool<FSendOverlapped>* _sendOverlapObjectPool = nullptr;
	CObjectPool<FTCPRecvOverlapped>* _tcpRecvOverlapObjectPool = nullptr;
	CObjectPool<FCloseOverlapped>* _closeOverlapObjectPool = nullptr;

	// 중복 close 처리를 막기위해 소켓마다 ON OFF 처리를 하는 map.
	std::map<CSocket*, bool> sockStates;
};
#endif