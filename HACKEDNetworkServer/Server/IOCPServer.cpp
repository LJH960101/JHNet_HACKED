#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib") 
#include "IOCPServer.h"
#include "Server/ServerNetworkSystem.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/Log.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/JHNETTool.h"
#include <WinSock2.h>
#include <MSWSock.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <tuple>
#include <chrono>
#include <ctime> 


using namespace JHNETTool;
using namespace JHNETSerializer;



CIOCPServer* CIOCPServer::_instance = nullptr;

CIOCPServer::CIOCPServer() : _bIsRun(false)
{}


CIOCPServer* CIOCPServer::GetInstance()
{
	if (!_instance) _instance = new CIOCPServer();
	return _instance;
}

CIOCPServer::~CIOCPServer()
{
	if (_acceptOverlapObjectPool) delete _acceptOverlapObjectPool;
	if (_sendOverlapObjectPool) delete _sendOverlapObjectPool;
	if (_tcpRecvOverlapObjectPool) delete _tcpRecvOverlapObjectPool;
}

void CIOCPServer::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::IOCPServer, level, msg);
}

bool CIOCPServer::Run()
{
	ServerNetworkSystem = CServerNetworkSystem::GetInstance();

	if (IsRun()) return false;
	_bIsRun = true;

	// Init WSA
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0 || !InitIOCP()) return false;

	// 초기화
	if (!InitObjectPools() || !InitTCPListenSocket() || !InitWorkerThread() || !InitUDPListenSocket()) {
		_bIsRun = false;
		return false;
	}

	// 첫 TCP Accept 시작
	PostAccept();

	// 첫 UDP Recv 시작
	PostRecv(new FUDPRecvOverlapped(), false);

	return true;
}

void CIOCPServer::PostClose(CSocket* socket)
{
	if (!socket || !socket->GetSocket() || sockStates[socket] == false) {
		ServerNetworkSystem->OnClose(socket);
		return;
	}

	// 같은 소켓을 또다시 단절하는 일이 없도록 체크한다.
	sockStates[socket] = false;

	FCloseOverlapped* overlap = _closeOverlapObjectPool->PopObject();
	overlap->socket = socket;

	// 단절을 요청한다.
	if (TransmitFile(overlap->socket->GetSocket(),
		0, 0, 0, overlap, 0, TF_REUSE_SOCKET) == FALSE)
	{
		DWORD error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			if(error != WSAENOTCONN) ServerNetworkSystem->socketObjectPool->ReturnObject(overlap->socket);
			WriteLog(ELogLevel::Error, CLog::Format("Failed to close. Error : %d", WSAGetLastError()));
			ServerNetworkSystem->OnClose(socket);
			_closeOverlapObjectPool->ReturnObject(overlap);
			return;
		}
	}
}

void CIOCPServer::PostAccept()
{
	FAcceptOverlapped* overlap = _acceptOverlapObjectPool->PopObject();
	CSocket* newSocket = ServerNetworkSystem->socketObjectPool->PopObject();

	overlap->socket = newSocket;

	WriteLog(ELogLevel::Warning, "Start accept.");

	// accept
	if (AcceptEx(_tcpListenSocket->GetSocket(), newSocket->GetSocket(), (LPVOID)&(overlap->m_acceptBuf), 0,
		sizeof(overlap->m_acceptBuf.m_pLocal), sizeof(overlap->m_acceptBuf.m_pRemote), &_tcpAcceptLen, overlap) == FALSE) {
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			PostClose(newSocket);
			WriteLog(ELogLevel::Error, CLog::Format("Failed to accept. Error : ", WSAGetLastError()));
			_bIsRun = false;
		}
	}
}

void CIOCPServer::PostRecv(FBuffuerableOverlapped* overlap, bool isTCP)
{
	if (isTCP) {
		if (!overlap) return;
		FTCPRecvOverlapped* tcpOverlap = (FTCPRecvOverlapped*)overlap;

		CSocket* socket = tcpOverlap->socket;
		if (socket == nullptr) {
			WriteLog(ELogLevel::Error, CLog::Format("Not exist socket!!"));

			// Close and logging
			_tcpRecvOverlapObjectPool->ReturnObject(tcpOverlap);
			ServerNetworkSystem->Close(tcpOverlap->socket);
			return;
		}

		// TCP는 WSARecv()를 사용하여 플레이어의 소켓에 Recv를 요청한다
		ZeroMemory(tcpOverlap, sizeof(WSAOVERLAPPED));

		tcpOverlap->wsabuf.len = PACKET_BUFSIZE;
		int retval = WSARecv(socket->GetSocket(), &tcpOverlap->wsabuf, 1,
			&recvBytes, &flags, tcpOverlap, NULL);
		if (retval == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				WriteLog(ELogLevel::Error, CLog::Format("WSARecv Error %d\n", WSAGetLastError()));

				// Close and logging
				_tcpRecvOverlapObjectPool->ReturnObject(tcpOverlap);
				ServerNetworkSystem->Close(tcpOverlap->socket);
				return;
			}
		}
	}
	else {
		FUDPRecvOverlapped* udpOverlap = nullptr;
		if (overlap) udpOverlap = (FUDPRecvOverlapped*)overlap;
		if (!udpOverlap) {
			udpOverlap = new FUDPRecvOverlapped();
			WriteLog(ELogLevel::Error, CLog::Format("UDP Recv overlap missing!!!!\n"));
		}
		ZeroMemory(udpOverlap, sizeof(WSAOVERLAPPED));

		// UDP는 WSARecvFrom을 사용하여, 플레이어의 Addr에 RecvFrom을 요청한다.
		udpOverlap->udpAddr = _udpServeraddr;
		udpOverlap->wsabuf.buf = udpOverlap->buf;
		udpOverlap->wsabuf.len = PACKET_BUFSIZE;

		if (WSARecvFrom(_udpRecvSocket, &udpOverlap->wsabuf, 1,
			&recvBytes, &flags, (sockaddr*)&udpOverlap->udpAddr, &UDPAddrLen, udpOverlap, NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				WriteLog(ELogLevel::Error, CLog::Format("WSARecvFrom Error %d\n", WSAGetLastError()));

				PostRecv((FBuffuerableOverlapped*)udpOverlap, false);
				return;
			}
		}
	}
}

void CIOCPServer::_AcceptProc(FAcceptOverlapped * overlap)
{
	// 소켓 옵션을 활성화 상태로 바꾼다.
	SOCKET listenSocket = _tcpListenSocket->GetSocket();

	if (setsockopt(overlap->socket->GetSocket(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		(const char *)&listenSocket, sizeof(SOCKET)) == SOCKET_ERROR)
	{
		std::string clientAcceptLog = CLog::Format("Can't set update accept context sockopt in _AcceptProc.");
		WriteLog(ELogLevel::Warning, clientAcceptLog);
		PostClose(overlap->socket);
		_acceptOverlapObjectPool->ReturnObject(overlap);
		return;
	}

	// Accept한 소켓의 addr을 받아온다.
	FSocketAddr *local_addr, *remote_addr;
	int l_len = 0, r_len = 0;
	GetAcceptExSockaddrs(&overlap->m_acceptBuf, 0, sizeof(overlap->m_acceptBuf.m_pLocal),
		sizeof(overlap->m_acceptBuf.m_pRemote), &local_addr, &l_len, &remote_addr, &r_len);
	FSocketAddrIn clientaddr;
	memcpy(&clientaddr, reinterpret_cast<FSocketAddrIn*>(remote_addr), sizeof(FSocketAddrIn));

	std::string clientAcceptLog = CLog::Format("[Accept] IP=%s, PORT=%d",
		inet_ntoa(clientaddr.sin_addr),
		ntohs(clientaddr.sin_port));
	WriteLog(ELogLevel::Warning, clientAcceptLog);

	CSocket* socket = overlap->socket;
	ServerNetworkSystem->OnAccept(overlap->socket, clientaddr);
	_acceptOverlapObjectPool->ReturnObject(overlap);
	sockStates[socket] = true;

	// 수신을 위한 overlapped를 준비한다.
	FTCPRecvOverlapped* recvOverlap = _tcpRecvOverlapObjectPool->PopObject();
	recvOverlap->wsabuf.len = PACKET_BUFSIZE;
	recvOverlap->socket = socket;

	// IOCP를 연결해준다.
	CreateIoCompletionPort((HANDLE)socket->GetSocket(), hcp, socket->GetSocket(), 0);

	// Recv를 실행한다.
	DWORD recvbytes, flags = 0;
	if (WSARecv(socket->GetSocket(), &recvOverlap->wsabuf, 1, &recvbytes,
		&flags, recvOverlap, NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			WriteLog(ELogLevel::Error, CLog::Format("Failed to recv : %d", WSAGetLastError()));
		}
	}

	// 다시 Accept을 준비한다.
	PostAccept();
}

void CIOCPServer::_RecvProc(FBuffuerableOverlapped* overlap, const int& len, bool isTCP)
{
	if (isTCP) {
		FTCPRecvOverlapped* tcpOverlap = (FTCPRecvOverlapped*)overlap;
		if (len <= 0 || len > PACKET_BUFSIZE) {
			WriteLog(ELogLevel::Error, CLog::Format("ReceiveData : Error %d", len));
			ServerNetworkSystem->Close(overlap->socket);

			_tcpRecvOverlapObjectPool->ReturnObject(tcpOverlap);
			return;
		}

		tcpOverlap->wsabuf.len = len;
		ServerNetworkSystem->OnRecvTCP(tcpOverlap->socket, tcpOverlap->buf, tcpOverlap->wsabuf.len);

		PostRecv(overlap, true);
	}
	else {
		FUDPRecvOverlapped* udpOverlap = (FUDPRecvOverlapped*)overlap;
		if (len > 0 && len <= PACKET_BUFSIZE) {
			udpOverlap->wsabuf.len = len;
			ServerNetworkSystem->OnRecvUDP(udpOverlap->udpAddr, udpOverlap->buf, udpOverlap->wsabuf.len);
		}
		else {
			WriteLog(ELogLevel::Error, "Receive UDP Failed.");
		}

		// UDP는 오류가나도 계속 진행한다.
		PostRecv(overlap, false);
	}
}

void CIOCPServer::_SendProc(FSendOverlapped* overlap)
{
	_sendOverlapObjectPool->ReturnObject(overlap);
}

void CIOCPServer::_CloseProc(FCloseOverlapped* overlap)
{
	// 오브젝트를 반납한다.
	WriteLog(ELogLevel::Warning, "Close End.");
	ServerNetworkSystem->OnClose(overlap->socket);
	ServerNetworkSystem->socketObjectPool->ReturnObject(overlap->socket);
	_closeOverlapObjectPool->ReturnObject(overlap);
}

FSocketAddrIn* CIOCPServer::ADDRToADDRIN(FSocketAddr* addr)
{
	return (FSocketAddrIn*)addr;
}

bool CIOCPServer::InitWorkerThread()
{
	// Check CPU
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// Create Worker Thread
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; ++i) {
	//for(int i=0; i<=0; ++i){
		hThread = CreateThread(NULL, 0, WorkerThread, (LPVOID*)this, 0, nullptr);
		if (hThread == nullptr) {
			WriteLog(ELogLevel::Error, "Failed to Create Thread");
			return false;
		}
		CloseHandle(hThread);
	}
	return true;
}

bool CIOCPServer::InitTCPListenSocket()
{
	// Create Listen Socket
	_tcpListenSocket = ServerNetworkSystem->socketObjectPool->PopObject();

	// Bind socket to IOCP
	CreateIoCompletionPort((HANDLE)_tcpListenSocket->GetSocket(), hcp, _tcpListenSocket->GetSocket(), 0);

	// Bind
	FSocketAddrIn serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(std::stoi(TCP_SERVER_PORT));
	if (bind(_tcpListenSocket->GetSocket(), (FSocketAddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "Failed to bind.");
		return false;
	}

	int enable = 0;
	if (setsockopt(_tcpListenSocket->GetSocket(), SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		WriteLog(ELogLevel::Error, CLog::Format("setsockopt so_conditional_accept Error : ", WSAGetLastError()));
		return false;
	}

	int rcvBufSize = PACKET_BUFSIZE;
	if (setsockopt(_tcpListenSocket->GetSocket(), SOL_SOCKET, SO_RCVBUF, (const char*)&rcvBufSize, sizeof(int)) == SOCKET_ERROR)
	{
		WriteLog(ELogLevel::Error, CLog::Format("setsockopt SO_RCVBUF Error : ", WSAGetLastError()));
		return false;
	}

	// listen
	if (listen(_tcpListenSocket->GetSocket(), SOMAXCONN) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "Failed to listen.");
		return false;
	}
	return true;
}

bool CIOCPServer::InitUDPListenSocket()
{
	_udpRecvSocket = WSASocket(PF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&_udpServeraddr, sizeof(FSocketAddrIn));
	_udpServeraddr.sin_family = AF_INET;
	_udpServeraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_udpServeraddr.sin_port = htons(std::stoi(UDP_SERVER_PORT));
	if (bind(_udpRecvSocket, (FSocketAddr*)&_udpServeraddr, sizeof(_udpServeraddr)) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "Failed to bind.");
		return false;
	}

	int rcvBufSize = PACKET_BUFSIZE;
	if (setsockopt(_udpRecvSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&rcvBufSize, sizeof(int)) == SOCKET_ERROR)
	{
		WriteLog(ELogLevel::Error, CLog::Format("setsockopt SO_RCVBUF Error : ", WSAGetLastError()));
		return false;
	}

	CreateIoCompletionPort((HANDLE)_udpRecvSocket, hcp, _udpRecvSocket, 0);

	return true;
}

bool CIOCPServer::InitIOCP()
{
	// Create IOCP
	hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == nullptr) {
		WriteLog(ELogLevel::Error, "Failed to Create IOCP");
		return false;
	}
	return true;
}

bool CIOCPServer::InitObjectPools()
{
	ServerNetworkSystem->socketObjectPool = new CObjectPool<CSocket>();
	_acceptOverlapObjectPool = new CObjectPool<FAcceptOverlapped>;
	_sendOverlapObjectPool = new CObjectPool<FSendOverlapped>;
	_tcpRecvOverlapObjectPool = new CObjectPool<FTCPRecvOverlapped>;
	_closeOverlapObjectPool = new CObjectPool<FCloseOverlapped>;

	ServerNetworkSystem->socketObjectPool->SetWithMutex(true);
	_acceptOverlapObjectPool->SetWithMutex(true);
	_sendOverlapObjectPool->SetWithMutex(true);
	_tcpRecvOverlapObjectPool->SetWithMutex(true);
	_closeOverlapObjectPool->SetWithMutex(true);

	return true;
}

DWORD WINAPI CIOCPServer::WorkerThread(LPVOID arg)
{
	int retval;
	CIOCPServer* owner = (CIOCPServer*)arg;
	HANDLE& hcp = owner->hcp;

	DWORD cbTransferred;
	SOCKET client_sock;
	FOverlapped* overlap;

	while (true) {
		// Wait until Async IO end
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, &client_sock,
			(LPOVERLAPPED *)&overlap, INFINITE);

		if (!overlap) continue;
		try {
			switch (overlap->m_type)
			{
			case EOverlappedType::ACCEPT:
			{
				owner->_AcceptProc((FAcceptOverlapped*)overlap);
				break;
			}
			case EOverlappedType::TCP_RECV:
			{
				owner->_RecvProc((FBuffuerableOverlapped*)overlap, cbTransferred, true);
				break;
			}
			case EOverlappedType::UDP_RECV:
			{
				owner->_RecvProc((FBuffuerableOverlapped*)overlap, cbTransferred, false);
				break;
			}
			case EOverlappedType::SEND:
			{
				owner->_SendProc((FSendOverlapped*)overlap);
				break;
			}
			case EOverlappedType::CLOSE:
			{
				owner->_CloseProc((FCloseOverlapped*)overlap);
				break;
			}
			}
		}
		catch (...) {
			owner->WriteLog(ELogLevel::Error, CLog::Format("[ReceiveData Exception]"));
			owner->ServerNetworkSystem->Close(overlap->socket);
			delete overlap;
		}
	}
	return 0;
}

void CIOCPServer::Shutdown()
{
	_bIsRun = false;
	WSACleanup();
}

void CIOCPServer::PostSend(CSocket* socket, const char* buf, const int& sendLen)
{
	if (!socket || sendLen < 0 || sendLen > PACKET_BUFSIZE) return;
	FSendOverlapped* overlap = _sendOverlapObjectPool->PopObject();

	// 앞에 총 길이를 더해서 보낸다.
	int realSendLen = sendLen + sizeof(int);

	// 버퍼를 만든다.
	JHNETSerializer::IntSerialize(overlap->wsabuf.buf, sendLen);
	memcpy(overlap->buf + sizeof(int), buf, sendLen);
	overlap->wsabuf.len = realSendLen;

	if (WSASend(socket->GetSocket(), &overlap->wsabuf, 1, &overlap->wsabuf.len, 0, overlap, NULL) == SOCKET_ERROR)		// 데이터 전송
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			WriteLog(ELogLevel::Error, CLog::Format("Failed to WSASend. Error : %d\n", WSAGetLastError()));
			ServerNetworkSystem->Close(socket);
			_sendOverlapObjectPool->ReturnObject(overlap);
			return;
		}
	}
}

void CIOCPServer::PostUDPSend(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen)
{
	if (sendLen < 0 || sendLen > PACKET_BUFSIZE) return;

	FSendOverlapped* overlap = _sendOverlapObjectPool->PopObject();

	// 앞에 총 길이를 더해서 보낸다.
	int realSendLen = sendLen + sizeof(int);

	// 버퍼를 만든다.
	overlap->wsabuf.buf = overlap->buf;
	JHNETSerializer::IntSerialize(overlap->buf, sendLen);
	memcpy(overlap->buf + sizeof(int), buf, sendLen);
	overlap->wsabuf.len = realSendLen;

	if (WSASendTo(_udpRecvSocket,
		&overlap->wsabuf, 1,
		&overlap->wsabuf.len, 0,
		(FSocketAddr*)&sockaddr, sizeof(FSocketAddrIn),
		overlap, NULL) == SOCKET_ERROR)		// 데이터 전송
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			WriteLog(ELogLevel::Error, CLog::Format("Failed to WSASendTo Error : ", WSAGetLastError()));
			return;
		}
	}
}

bool CIOCPServer::IsRun()
{
	return _bIsRun;
}

void CIOCPServer::Send(CSocket* socket, const char* buf, const int& sendLen)
{
	PostSend(socket, buf, sendLen);
}

void CIOCPServer::Close(CSocket* socket)
{
	WriteLog(ELogLevel::Warning, "Close Start.");
	PostClose(socket);
	//CloseSocket(socket);
}

void CIOCPServer::SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen)
{
	PostUDPSend(sockaddr, buf, sendLen);
}

void CIOCPServer::CloseSocket(CSocket* socket)
{
	if (!socket || !socket->GetSocket() || sockStates[socket] == false) return;

	// 같은 소켓을 또다시 단절하는 일이 없도록 체크한다.
	sockStates[socket] = false;

	closesocket(socket->GetSocket());
	socket->InitSocket();
}

void CIOCPServer::PrintObjectPools()
{
	printf("CIOCPServer::PrintObjectPools");
	printf("_acceptOverlapObjectPool : ");
	_acceptOverlapObjectPool->Print();
	printf("_sendOverlapObjectPool : ");
	_sendOverlapObjectPool->Print();
	printf("_tcpRecvOverlapObjectPool : ");
	_tcpRecvOverlapObjectPool->Print();
	printf("_closeOverlapObjectPool : ");
	_closeOverlapObjectPool->Print();
}

#endif