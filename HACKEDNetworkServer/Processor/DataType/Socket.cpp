#include "Socket.h"
#include "NetworkModule/Log.h"
#include <stdio.h>
#include "Server/ServerNetworkSystem.h"

CSocket::CSocket() {
	InitSocket();
}

CSocket::~CSocket()
{

}

CSocket::CSocket(SOCKET socket)
{
	_socket = socket;
}

SOCKET CSocket::GetSocket()
{
	return _socket;
}

void CSocket::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::Socket, level, msg);
}

const int SOCKET_OPTION_CONDITIONAL_ACCEPT = 0x3002;
void CSocket::InitSocket()
{
#ifdef _WIN32
	_socket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_socket == -1) {
		WriteLog(ELogLevel::Error, "Failed to create socket.");
		return;
	}

	bool on = true;
	if (setsockopt(_socket, SOL_SOCKET, SOCKET_OPTION_CONDITIONAL_ACCEPT, (char*)& on, sizeof(on)))
	{
		WriteLog(ELogLevel::Error, "Can't set SO_CONDITIONAL_ACCEPT.");
		return;
	}
#endif
}