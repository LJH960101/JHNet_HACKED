#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "IOCPServer.h"
#include "EpollServer.h"
#include "Server/ServerNetworkSystem.h"
#include "Processor/UDP/UDPReceiveProcessor.h"
#include "NetworkModule/Log.h"
#include "Processor/DataType/Socket.h"
#include "Processor/GameProcessor.h"
#include "NetworkModule/Serializer.h"
#include <iostream>

using namespace JHNETSerializer;

CServerNetworkSystem* CServerNetworkSystem::_instance = nullptr;
CServerNetworkSystem::CServerNetworkSystem()
{
#ifdef _WIN32 // WINDOW
	_server = CIOCPServer::GetInstance();
#elif __linux__ // LINUX
	_server = CEpollServer::GetInstance();
#endif
}

CServerNetworkSystem * CServerNetworkSystem::GetInstance()
{
	if (_instance == nullptr) _instance = new CServerNetworkSystem();
	return _instance;
}

CServerNetworkSystem::~CServerNetworkSystem()
{
}

bool CServerNetworkSystem::Run()
{
	std::chrono::seconds sleepDuration(2);
	WriteLog(ELogLevel::Warning, "Run statred....");

	// Run Server
	if (!_server->Run()) return false;

	GameProcessor = new CGameProcessor();
	GameProcessor->Run();

	return true;
}

void CServerNetworkSystem::Shutdown()
{
	_server->Shutdown();
	GameProcessor->Shutdown();

	if(socketObjectPool) delete socketObjectPool;
}

void CServerNetworkSystem::Send(CSocket* socket, const char* buf, const int& sendLen)
{
	_server->Send(socket, buf, sendLen);
}

void CServerNetworkSystem::Close(CSocket* socket)
{
	_server->Close(socket);
}

void CServerNetworkSystem::SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen)
{
	_server->SendTo(sockaddr, buf, sendLen);
}

bool CServerNetworkSystem::IsRun()
{
	return _server->IsRun();
}

void CServerNetworkSystem::OnAccept(CSocket* socket, const FSocketAddrIn& clientAddr)
{
	GameProcessor->PostAcceptMessage(socket, clientAddr);
}

void CServerNetworkSystem::OnRecvTCP(CSocket* socket, char* buf, const int& len)
{
	GameProcessor->PostTCPMessage(socket, buf, len);
}

void CServerNetworkSystem::OnRecvUDP(const FSocketAddrIn& udpAddr, char* buf, const int& len)
{
	GameProcessor->PostUDPMessage(udpAddr, buf, len);
}

void CServerNetworkSystem::OnClose(CSocket* socket)
{
	GameProcessor->PostCloseMessage(socket);
}

void CServerNetworkSystem::PrintObjectPools()
{
	printf("_socketObjects : ");
	socketObjectPool->Print();
	_server->PrintObjectPools();
	GameProcessor->PrintObjectPools();
}

void CServerNetworkSystem::Print()
{
	GameProcessor->Print();
}

void CServerNetworkSystem::PrintSummary()
{
	GameProcessor->PrintSummary();
}

void CServerNetworkSystem::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::ServerNetworkSystem, level, msg);
}