#pragma once
#include "Processor/DataType/Player.h"
#include "Processor/DataType/Socket.h"

class IServer {
public:
	virtual bool Run() = 0;
	virtual void Send(CSocket* socket, const char* buf, const int& sendLen) = 0;
	virtual void Close(CSocket* socket) = 0;
	virtual void SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen) = 0;
	virtual bool IsRun() = 0;
	virtual void Shutdown() = 0;
	virtual void PrintObjectPools() = 0;
};