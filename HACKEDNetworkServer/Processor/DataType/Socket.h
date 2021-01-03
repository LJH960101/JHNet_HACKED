#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "JHNETGameServer.h"
#include "NetworkModule/Log.h"

#ifdef __linux__ // Linux Socket
typedef sockaddr FSocketAddr;
typedef sockaddr_in FSocketAddrIn;
typedef int SOCKET;

#elif _WIN32 // Window Socket
typedef SOCKADDR FSocketAddr;
typedef SOCKADDR_IN FSocketAddrIn;
#endif

class CSocket
{
public:
	CSocket();
	~CSocket();
	CSocket(SOCKET socket);
	operator SOCKET()
	{
		return _socket;
	}

	SOCKET GetSocket();
	void InitSocket();
private:
	SOCKET _socket;
	static void WriteLog(const ELogLevel& level, const std::string& msg);
};