#pragma once
#ifdef _WIN32
#include <WinSock2.h>
#include "Processor/DataType/Socket.h"
#include "JHNETGameServer.h"
#include <WinSock2.h>

enum class EOverlappedType
{
	ACCEPT,
	TCP_RECV,
	UDP_RECV,
	SEND,
	CLOSE
};

struct FOverlapped : public WSAOVERLAPPED
{
	FOverlapped()
	{
		ZeroMemory(this, sizeof(FOverlapped));
	}
	CSocket* socket;
	EOverlappedType m_type;
};

struct FAcceptOverlapped : public FOverlapped
{
	struct _accept {
		BYTE m_pLocal[sizeof(FSocketAddrIn) + 16];
		BYTE m_pRemote[sizeof(FSocketAddrIn) + 16];
	} m_acceptBuf; // Accept에 사용하는 버퍼

	FAcceptOverlapped() : FOverlapped()
	{
		m_type = EOverlappedType::ACCEPT;
	}
};

struct FCloseOverlapped : public FOverlapped
{
	FCloseOverlapped() : FOverlapped()
	{
		m_type = EOverlappedType::CLOSE;
	}
};

struct FBuffuerableOverlapped : public FOverlapped
{
	FBuffuerableOverlapped() : FOverlapped()
	{
		wsabuf.buf = buf;
	}
	FBuffuerableOverlapped& operator = (const FBuffuerableOverlapped &rhs) {
		m_type = rhs.m_type;
		return *this;
	}
	WSABUF wsabuf;
	char buf[PACKET_BUFSIZE];
};

struct FTCPRecvOverlapped : public FBuffuerableOverlapped
{
	FTCPRecvOverlapped() : FBuffuerableOverlapped()
	{
		m_type = EOverlappedType::TCP_RECV;
	}
	FTCPRecvOverlapped& operator = (const FTCPRecvOverlapped &rhs) {
		m_type = rhs.m_type;
		return *this;
	}
};

struct FUDPRecvOverlapped : public FBuffuerableOverlapped
{
	FUDPRecvOverlapped() : FBuffuerableOverlapped()
	{
		m_type = EOverlappedType::UDP_RECV;
	}
	FUDPRecvOverlapped& operator = (const FUDPRecvOverlapped &rhs) {
		m_type = rhs.m_type;
		udpAddr = rhs.udpAddr;
		return *this;
	}
	FSocketAddrIn udpAddr;		// UDP에서 사용하는 ADDR
};

struct FSendOverlapped : public FBuffuerableOverlapped
{
	FSendOverlapped() : FBuffuerableOverlapped()
	{
		m_type = EOverlappedType::SEND;
	}
};
#endif