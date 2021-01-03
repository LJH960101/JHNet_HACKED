
#ifdef __linux__
#include "EpollServer.h"
#include "Server/ServerNetworkSystem.h"
#include "Processor/TCP/TCPReceiveProcessor.h"
#include "Processor/UDP/UDPReceiveProcessor.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/Log.h"
#include "NetworkModule/Serializer.h"
#include "Processor/Manager/PlayerManager.h"
#include "Processor/Manager/RoomManager.h"
#include "Processor/Manager/PlayerManager.h"
#include "NetworkModule/JHNETTool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <tuple>
#include <chrono>
#include <ctime> 
#include <memory.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>

CEpollServer* CEpollServer::_instance = nullptr;

#define MAXEVENTS 500

bool CEpollServer::Run()
{
	RoomManager = CRoomManager::GetInstance();
	PlayerManager = GameProcessor->PlayerManager;
	ServerNetworkSystem = CServerNetworkSystem::GetInstance();

	if (IsRun()) return false;
	_bIsRun = true;

	if (!_InitEpoll() || !_InitTCPListenSocket() || !_InitUDPSocket() || !_InitWorkerThread()) return false;

	return true;
}

void CEpollServer::Shutdown()
{
	_bIsRun = false;
	close(_tcpListenSocket);
	close(_udpSocket);
	close(_epollFd);
}

void CEpollServer::Send(CPlayer* player, const char* buf, const int& sendLen, bool isTCP)
{
	FBuf* newFBuf = _bufObjectPool->PopObject();
	char* newBuf = newFBuf->buf;

	// 앞에 총 길이를 더해서 보낸다.
	JHNETSerializer::IntSerialize(newBuf, sendLen);
	memcpy(newBuf + (int)sizeof(int), buf, sendLen);
	int newLen = sendLen + (int)sizeof(int);

	if (isTCP || !player->IsHaveUDP()) {
		if (send(player->socket->GetSocket(), newBuf, newLen, 0)
			== SOCKET_ERROR) {
			WriteLog(ELogLevel::Error, CLog::Format("Send: errno = %d\n", errno));
			ServerNetworkSystem->CloseConnection(player);
			_bufObjectPool->ReturnObject(newFBuf);
			return;
		}
	}
	else {
		int retval = sendto(_udpSocket, newBuf, newLen, 0,
			(FSocketAddr*)&player->udpAddr, sizeof(FSocketAddrIn));
		if (retval == SOCKET_ERROR) {
			if (player) ServerNetworkSystem->CloseConnection(player);
			WriteLog(ELogLevel::Error, CLog::Format("SendTo: errno = %d\n", errno));
			_bufObjectPool->ReturnObject(newFBuf);
			return;
		}
	}
	_bufObjectPool->ReturnObject(newFBuf);
}

void CEpollServer::SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen)
{
	// 버퍼를 만든다.
	FBuf* newFBuf = _bufObjectPool->PopObject();
	char* newBuf = newFBuf->buf;
	JHNETSerializer::IntSerialize(newBuf, sendLen);
	memcpy(newBuf + sizeof(int), buf, sendLen);
	int newLen = sendLen + (int)sizeof(int);

	sendto(_udpSocket, newBuf, newLen, 0, (FSocketAddr*)&sockaddr, sizeof(FSocketAddrIn));
	_bufObjectPool->ReturnObject(newFBuf);
}

void CEpollServer::Close(CSocket* socket)
{
	if (!socket || !socket->GetSocket() || sockStates[socket] == false) return;
	sockStates[socket] = false;

	_CloseSocket(socket->GetSocket());
	delete socket;
}

void CEpollServer::_CloseSocket(SOCKET socket)
{
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, socket, NULL);
	close(socket);
}

void CEpollServer::_WorkerThread(CEpollServer* owner)
{
	/* Buffer where events are returned */
	epoll_event* events;
	events = (epoll_event*)calloc(MAXEVENTS, sizeof(epoll_event));

	/* The event loop */
	while (owner->_bIsRun) {
		int n, i;
		n = epoll_wait(owner->_epollFd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++) {
			owner->_OnEvent(events[i]);
		}
	}

	free(events);
	owner->Shutdown();
	return;
}

void CEpollServer::_AcceptProc(const epoll_event& newEvent)
{
	while (_bIsRun) {
		struct sockaddr in_addr;
		socklen_t in_len;
		int infd;
		char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
		in_len = sizeof in_addr;
		infd = accept(_tcpListenSocket, &in_addr, &in_len);
		if (infd == SOCKET_ERROR) {
			if ((errno == EAGAIN) ||
				(errno == EWOULDBLOCK)) {
				/* We have processed all incoming
				   connections. */
				break;
			}
			else {
				perror("accept");
				break;
			}
		}
		int retval = getnameinfo(&in_addr, in_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
		if (retval == 0) {
			WriteLog(ELogLevel::Warning, CLog::Format("Accepted connection on descriptor %d (host=%s, port=%s)\n", infd, hbuf, sbuf));

			// 플레이어를 생성한다.
			int addrlen = (int)sizeof(FSocketAddr);
			FSocketAddrIn addr;
			CSocket* socket = new CSocket(infd);
			getpeername(socket->GetSocket(), (FSocketAddr*)& addr, (socklen_t*)& addrlen);
			ServerNetworkSystem->OnAccept(new CSocket(infd), addr);
		}
		/* Make the incoming socket non-blocking and add it to the
		   list of fds to monitor. */
		retval = _SetNonBlockingSocket(infd);
		if (retval == SOCKET_ERROR)
			abort();
		_epollEvent.data.fd = infd;
		_epollEvent.events = EPOLLIN | EPOLLET;
		retval = epoll_ctl(_epollFd, EPOLL_CTL_ADD, infd, &_epollEvent);
		if (retval == SOCKET_ERROR) {
			perror("epoll_ctl");
			abort();
		}
	}
}

void CEpollServer::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::EpollServer, level, msg);
}

int CEpollServer::_SetNonBlockingSocket(const SOCKET & socket)
{
	int flag;
	flag = fcntl(socket, F_GETFL, 0);
	if (flag == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "SetNonBlockingSocket : F_GETFL error.");
		return SOCKET_ERROR;
	}
	flag |= O_NONBLOCK;
	if (fcntl(socket, F_SETFL, flag) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "SetNonBlockingSocket : F_SETFL error.");
		return SOCKET_ERROR;
	}
	return 0;
}

SOCKET CEpollServer::_CreateSocket(const char * port, bool isTCP)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, socketFd;

	// AddrInfo를 받기위한 hints 선언
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;

	if (isTCP) hints.ai_socktype = SOCK_STREAM;
	else hints.ai_socktype = SOCK_DGRAM;

	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		WriteLog(ELogLevel::Error, CLog::Format("CreateSocket : getaddrinfo error %s\n", gai_strerror(s)));
		return SOCKET_ERROR;
	}

	// 가능한 소켓들에다 Bind를 걸어본다.
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		socketFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (socketFd == SOCKET_ERROR)
			continue;
		s = bind(socketFd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			// Bind 성공!
			break;
		}

		// 실패시 재시도 해봄.
		WriteLog(ELogLevel::Error, CLog::Format("CreateSocket : bind error %d\n", errno));
		close(socketFd);
	}

	// 가능한 소켓이 없다.
	if (rp == NULL) {
		WriteLog(ELogLevel::Error, CLog::Format("CreateSocket : Can't bind socket...\n"));
		return SOCKET_ERROR;
	}
	freeaddrinfo(result);

	// 최대 버퍼크기 설정.
	int opt = PACKET_BUFSIZE;
	if (setsockopt(socketFd, SOL_SOCKET, SO_RCVBUF, (const char*)&opt, sizeof(int)) == SOCKET_ERROR)
	{
		WriteLog(ELogLevel::Warning, CLog::Format("setsockopt SO_RCVBUF Error."));
		return false;
	}

	// No Delay 설정.
	opt = 1;
	if (isTCP && setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Warning, CLog::Format("setsockopt TCP_NODELAY Error."));
		return false;
	}

	return socketFd;
}

CEpollServer* CEpollServer::GetInstance()
{
	if (!_instance) _instance = new CEpollServer();
	return _instance;
}

CEpollServer::CEpollServer() : _bIsRun(false)
{
	_tcpAdrSize = sizeof(FSocketAddrIn);
	_bufObjectPool = new CObjectPool<FBuf>();
}

void CEpollServer::_OnEvent(const epoll_event& newEvent)
{
	if ((newEvent.events & EPOLLERR) ||
		(newEvent.events & EPOLLHUP) ||
		(!(newEvent.events & EPOLLIN))) {
		_ErrorProc(newEvent);
		return;
	}
	// UDP Recv
	else if (_udpSocket == newEvent.data.fd) {
		_UDPRecvProc(newEvent);
	}
	// TCP Accept
	else if (_tcpListenSocket == newEvent.data.fd) {
		_AcceptProc(newEvent);
		return;
	}
	else {
		_TCPRecvProc(newEvent);
	}
}

bool CEpollServer::_InitEpoll()
{
	_epollFd = epoll_create1(0);
	if (_epollFd == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitEpoll : Failed to epoll_create1.");
		return false;
	}
	return true;
}

bool CEpollServer::_InitTCPListenSocket()
{
	_tcpListenSocket = _CreateSocket(TCP_SERVER_PORT, true);
	if (_tcpListenSocket == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitTCPListenSocket : Failed to create_and_bind.");
		return false;
	}
	if (_SetNonBlockingSocket(_tcpListenSocket) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitTCPListenSocket : Failed to _SetNonBlockingSocket.");
		return false;
	}
	if (listen(_tcpListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitTCPListenSocket : Failed to listen.");
		return false;
	}

	_epollEvent.data.fd = _tcpListenSocket;
	_epollEvent.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _tcpListenSocket, &_epollEvent) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitTCPListenSocket : Failed to epoll_ctl.");
		return false;
	}
	return true;
}

bool CEpollServer::_InitUDPSocket()
{
	_udpSocket = _CreateSocket(UDP_SERVER_PORT, false);

	if (_udpSocket == SOCKET_ERROR)
		abort();
	if (_SetNonBlockingSocket(_udpSocket) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitUDPSocket : _SetNonBlockingSocket error!!");
		return false;
	}

	epoll_event udpEvent;
	udpEvent.data.fd = _udpSocket;
	udpEvent.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _udpSocket, &udpEvent) == SOCKET_ERROR) {
		WriteLog(ELogLevel::Error, "InitUDPSocket : epoll_ctl error!!");
		return false;
	}
	return true;
}

void CEpollServer::_ErrorProc(const epoll_event& newEvent)
{
	WriteLog(ELogLevel::Error, "Epoll error");
	
	CPlayer* player = PlayerManager->GetPlayerBySocket(newEvent.data.fd);
	if(player) ServerNetworkSystem->CloseConnection(player);
	// TCP 클라이언트 소켓일 경우 Close 처리
	else if (newEvent.data.fd != _tcpListenSocket &&
		newEvent.data.fd != _udpSocket) {
		_CloseSocket(newEvent.data.fd);
	}

	return;
}

void CEpollServer::_TCPRecvProc(const epoll_event& newEvent)
{
	CPlayer* player = PlayerManager->GetPlayerBySocket(newEvent.data.fd);

	ssize_t bufLen = 0;

	FBuf* newFBuf = _bufObjectPool->PopObject();
	char* newBuf = newFBuf->buf;
	while (_bIsRun) {
		// 읽기 수행!
		bufLen = read(newEvent.data.fd, newBuf, PACKET_BUFSIZE);
		if (bufLen == SOCKET_ERROR) {
			// 오류 발생
			if (errno != EAGAIN) {
				WriteLog(ELogLevel::Error, "TCPRecvProc : read error!!");
				if(player) ServerNetworkSystem->CloseConnection(player);
				else _CloseSocket(newEvent.data.fd);
				break;
			}
			// 아니라면 read 끝
			break;
		}
		else if (bufLen == 0) {
			// 커넥션이 끊어짐.
			WriteLog(ELogLevel::Warning, "TCPRecvProc : Connection lost.");
			if (player) ServerNetworkSystem->CloseConnection(player);
			else _CloseSocket(newEvent.data.fd);
			break;
		}

		// 수신!
		if (!CTCPReceiveProcessor::GetInstance()->ReceiveData(player, newBuf, (int)bufLen)) {
			std::string errorLog = CLog::Format("[TCP ReceiveData Error] %s\n", inet_ntoa(player->addr.sin_addr));
			WriteLog(ELogLevel::Error, errorLog);
			ServerNetworkSystem->CloseConnection(player);
			break;
		}

		// 해당 소켓에 읽기가 끝날때까지 재수행한다.
		read(newEvent.data.fd, newBuf, PACKET_BUFSIZE);
	}
	_bufObjectPool->ReturnObject(newFBuf);
}

void CEpollServer::_UDPRecvProc(const epoll_event& newEvent)
{
	FSocketAddrIn addr;
	socklen_t addr_size = sizeof(FSocketAddrIn);

	ssize_t bufLen;
	FBuf* newFBuf = _bufObjectPool->PopObject();
	char* newBuf = newFBuf->buf;
	while (_bIsRun) {
		memset(&addr, 0, sizeof(FSocketAddrIn));

		// 읽기 수행!
		bufLen = recvfrom(_udpSocket, newBuf, PACKET_BUFSIZE, 0, (FSocketAddr *)&addr, &addr_size);
		if (bufLen == SOCKET_ERROR) {
			// 오류 발생
			if (errno != EAGAIN) {
				WriteLog(ELogLevel::Error, "UDPRecvProc : Connection lost.");

				CPlayer* player = PlayerManager->GetPlayerByUDPAddr(addr);
				if (player) ServerNetworkSystem->CloseConnection(player);

				break;
			}
			// Read 끝!
			break;
		}
		else if (bufLen == 0) {
			// 커넥션이 끊어짐.

			CPlayer* player = PlayerManager->GetPlayerByUDPAddr(addr);
			if(player) ServerNetworkSystem->CloseConnection(player);

			break;
		}

		// 수신!
		if (!CUDPReceiveProcessor::GetInstance()->ReceiveData(addr, newBuf, (int)bufLen)) {
			std::string errorLog = CLog::Format("[UDP ReceiveData Error] %s\n", inet_ntoa(addr.sin_addr));
			WriteLog(ELogLevel::Error, errorLog);

			CPlayer* player = PlayerManager->GetPlayerByUDPAddr(addr);
			if(player) ServerNetworkSystem->CloseConnection(player);
			break;
		}

		// 쌓인 읽기가 끝날때까지 재수행.
		read(newEvent.data.fd, newBuf, PACKET_BUFSIZE);
	}
	_bufObjectPool->ReturnObject(newFBuf);
}

bool CEpollServer::_InitWorkerThread()
{
	_workerThread = new std::thread(_WorkerThread, this);
	if (_workerThread) return true;
	else return false;
}

CEpollServer::~CEpollServer()
{
	if (_workerThread) {
		delete _workerThread;
		_workerThread = nullptr;
	}
	if (_bufObjectPool) {
		delete _bufObjectPool;
		_bufObjectPool = nullptr;
	}
}

bool CEpollServer::IsRun()
{
	return _bIsRun;
}

void CEpollServer::PrintObjectPools()
{
	printf("_bufObjectPool : ");
	_bufObjectPool->Print();
}

#endif