#include "GameProcessor.h"
#include "Server/ServerNetworkSystem.h"

bool CGameProcessor::Run()
{
	_isRun = true;

	_acceptMsgObjectPool = new CObjectPool<FAcceptMessage>();
	_tcpMsgObjectPool = new CObjectPool<FTCPMessage>();
	_udpMsgObjectPool = new CObjectPool<FUDPMessage>();
	_closeMsgObjectPool = new CObjectPool<FCloseMessage>();
	bufObjectPool = new CObjectPool<char>();
	_messageQueue = new CMessageQueue<FGameProcessorMessage*>();
	_currentQueue = _messageQueue->CreateQueue();

	_acceptMsgObjectPool->SetWithMutex(true);
	_tcpMsgObjectPool->SetWithMutex(true);
	_udpMsgObjectPool->SetWithMutex(true);
	_closeMsgObjectPool->SetWithMutex(true);
	bufObjectPool->SetWithMutex(false);

	PlayerManager = new CPlayerManager();
	RoomManager = new CRoomManager();
	TCPReceiveProcessor = new CTCPReceiveProcessor();
	UDPReceiveProcessor = new CUDPReceiveProcessor();

	ServerNetworkSystem = CServerNetworkSystem::GetInstance();
	PlayerManager->SetGameProcessor(this);
	RoomManager->SetGameProcessor(this);
	TCPReceiveProcessor->SetGameProcessor(this);
	UDPReceiveProcessor->SetGameProcessor(this);

	_logicThread = new std::thread(&CGameProcessor::GameLogicThread, this);

	return true;
}

void CGameProcessor::Shutdown()
{
	_isRun = false;
	if (_logicThread) {
		_logicThread->join();
		delete _logicThread;
		_logicThread = nullptr;
	}
	if (_acceptMsgObjectPool) delete _acceptMsgObjectPool;
	if (_tcpMsgObjectPool) delete _tcpMsgObjectPool;
	if (_udpMsgObjectPool) delete _udpMsgObjectPool;
	if (_closeMsgObjectPool) delete _closeMsgObjectPool;
	if (_messageQueue) delete _messageQueue;
	if (_currentQueue) delete _currentQueue;
	if (bufObjectPool) delete bufObjectPool;
	if (PlayerManager) delete PlayerManager;
	if (RoomManager) delete RoomManager;
	if (TCPReceiveProcessor) delete TCPReceiveProcessor;
	if (UDPReceiveProcessor) delete UDPReceiveProcessor;
}

void CGameProcessor::GameLogicThread()
{
	std::chrono::milliseconds sleepDuration(10);

	FGameProcessorMessage* msg;
	while (_isRun) {
		// 큐를 교체한다.
		_messageQueue->UpdateQueue(_currentQueue);

		// 큐가 빌때까지
		while (!_currentQueue->empty()) {
			// 맨 앞의 메세지를 사용한다.
			msg = _currentQueue->front();
			_currentQueue->pop();
			if (msg) {
				switch (msg->type) {
				case FGameProcessorMessage::EMessageType::ACCEPT:
					OnAcceptMessage((FAcceptMessage*)msg);
					break;
				case FGameProcessorMessage::EMessageType::TCP:
					OnTCPMessage((FTCPMessage*)msg);
					break;
				case FGameProcessorMessage::EMessageType::UDP:
					OnUDPMessage((FUDPMessage*)msg);
					break;
				case FGameProcessorMessage::EMessageType::CLOSE:
					OnCloseMessage((FCloseMessage*)msg);
					break;
				default:
					WriteLog(ELogLevel::Error, "Not exist type!!");
					break;
				}
			}
		}
		Update();
		std::this_thread::sleep_for(sleepDuration);
	}
}

void CGameProcessor::Update()
{
	int num = -1;
	while (true) {
		++num;
		try {
			std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
			CPlayer* player = PlayerManager->GetPlayerByNum(num);
			if (player == nullptr) break;
			std::chrono::seconds pingDelay = std::chrono::duration_cast<std::chrono::seconds>(currentTime - player->lastPingTime);
			std::chrono::seconds pongDelay = std::chrono::duration_cast<std::chrono::seconds>(currentTime - player->lastPongTime);
			// 일정 시간 이상 응답이 없다면 연결을 끊는다.
			if (player->state != ConnectionLost && pongDelay.count() >= PING_FINAL_DELAY) {
				WriteLog(ELogLevel::Error, CLog::Format("[%s:%d] : Close Connection By Ping FAILED.\n", inet_ntoa(player->addr.sin_addr),
					ntohs(player->addr.sin_port)));
				Close(player->socket);
			}
			// 스팀아이디가 없다면 핑대신 스팀 요청을 보낸다.
			else if (player->state != ConnectionLost && player->steamID == 0 && pingDelay.count() >= NONE_STEAM_PING_DELAY) {
				WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : STEAM REQUEST! \n", inet_ntoa(player->addr.sin_addr),
					ntohs(player->addr.sin_port)));

				FS_Common_RequestID sCommonRequestID;
				Send(player->socket, (FPacketStruct*)& sCommonRequestID);
				player->lastPingTime = currentTime;
			}
			// 스팀아이디가 있다면 핑만 보낸다.
			else if (player->state != ConnectionLost && player->steamID != 0 && pingDelay.count() >= PING_DELAY) {
				// SteamID는 등록되었지만 UDP가 등록되어 있지 않다면 등록을 요청한다.
				if (!player->IsHaveUDP()) {
					FS_UDP_UDPReconnectRequest udpReconnectRequest;
					Send(player->socket, (FPacketStruct*)& udpReconnectRequest);
				}
				else {
					WriteLog(ELogLevel::Warning, CLog::Format("[%s:%d] : PING! \n", inet_ntoa(player->addr.sin_addr),
						ntohs(player->addr.sin_port)));

					F_Common_Ping commonPing;
					Send(player->socket, (FPacketStruct*)& commonPing);
					player->lastPingTime = currentTime;
				}
			}

		}
		catch (std::exception& e) {
			// throw을 삼키고, 로그를 출력후 계속 진행.
			std::string newLog = CLog::Format("[ServerProcessThread:Exception] : %s", e.what());
			printf("%s\n", newLog.c_str());
			CLog::WriteLog(ELogType::ServerNetworkSystem, ELogLevel::Error, newLog);
		}
	}

	RoomManager->Update();
}

void CGameProcessor::PostAcceptMessage(CSocket* socket, const FSocketAddrIn& clientAddr)
{
	FAcceptMessage* acceptMessage = _acceptMsgObjectPool->PopObject();
	acceptMessage->socket = socket;
	acceptMessage->clientAddr = clientAddr;

	_messageQueue->Enqueue((FGameProcessorMessage*)acceptMessage);
}

void CGameProcessor::PostTCPMessage(CSocket* socket, char* buf, const int& len)
{
	if (len > PACKET_BUFSIZE || len <= 0) return;

	FTCPMessage* tcpMessage = _tcpMsgObjectPool->PopObject();
	memcpy(tcpMessage->buf, buf, len);
	tcpMessage->len = len;
	tcpMessage->socket = socket;

	_messageQueue->Enqueue((FGameProcessorMessage*)tcpMessage);
}

void CGameProcessor::PostUDPMessage(const FSocketAddrIn& addr, char* buf, const int& len)
{
	if (len > PACKET_BUFSIZE || len <= 0) return;

	FUDPMessage* udpMessage = _udpMsgObjectPool->PopObject();
	memcpy(udpMessage->buf, buf, len);
	udpMessage->len = len;
	udpMessage->udpAddr = addr;

	_messageQueue->Enqueue((FGameProcessorMessage*)udpMessage);
}

void CGameProcessor::PostCloseMessage(CSocket* socket)
{
	FCloseMessage* closeMessage = _closeMsgObjectPool->PopObject();
	closeMessage->socket = socket;

	_messageQueue->Enqueue((FGameProcessorMessage*)closeMessage);
}

void CGameProcessor::PrintObjectPools()
{
	printf("CGameProcessor::PrintObjectPools\n");
	printf("_acceptMsgObjectPool : ");
	_acceptMsgObjectPool->Print();
	printf("_tcpMsgObjectPool : ");
	_tcpMsgObjectPool->Print();
	printf("_udpMsgObjectPool : ");
	_udpMsgObjectPool->Print();
	printf("_closeMsgObjectPool : ");
	_closeMsgObjectPool->Print();
	printf("bufObjectPool : ");
	bufObjectPool->Print();

	PlayerManager->PrintObjectPools();
	RoomManager->PrintObjectPools();
}

void CGameProcessor::Print()
{
	PlayerManager->Print();
	RoomManager->Print();
}

void CGameProcessor::PrintSummary()
{
	RoomManager->PrintSummary();
	PlayerManager->PrintSummary();
}

void CGameProcessor::OnAcceptMessage(FAcceptMessage* msg)
{
	// Accept 처리
	// 플레이어를 생성한다.
	int addrlen = sizeof(msg->clientAddr);
	CPlayer* newPlayer = PlayerManager->CreatePlayer();
	newPlayer->socket = msg->socket;
	newPlayer->addr = msg->clientAddr;

	// 새로운 방을 만들어서 플레이어에게 할당한다.
	RoomManager->CreateRoom(newPlayer);

	_acceptMsgObjectPool->ReturnObject(msg);
}

void CGameProcessor::OnTCPMessage(FTCPMessage* msg)
{
	CPlayer* player = PlayerManager->GetPlayerBySocket(msg->socket);
	if (player) {
		// TCP 처리
		if (!TCPReceiveProcessor->ReceiveData(player, msg->buf, msg->len)) {
			std::string errorLog = CLog::Format("[TCP ReceiveData Error] %s\n", inet_ntoa(player->addr.sin_addr));
			WriteLog(ELogLevel::Error, errorLog);
			Close(msg->socket);
		}
	}

	_tcpMsgObjectPool->ReturnObject(msg);
}

void CGameProcessor::OnUDPMessage(FUDPMessage* msg)
{
	// UDP 처리
	if (!UDPReceiveProcessor->ReceiveData(msg->udpAddr, msg->buf, msg->len)) {
		std::string errorLog = CLog::Format("[UDP ReceiveData Error] %s\n", inet_ntoa(msg->udpAddr.sin_addr));
		WriteLog(ELogLevel::Error, errorLog);
	}

	_udpMsgObjectPool->ReturnObject(msg);
}

void CGameProcessor::OnCloseMessage(FCloseMessage* msg)
{
	// Close 처리
	CPlayer* player = PlayerManager->GetPlayerBySocket(msg->socket);
	if (!player) {
		_closeMsgObjectPool->ReturnObject(msg);
		return;
	}

	// 방에 단절을 통보한다.
	RoomManager->DisconnectRoom(player);

	std::string closeLog = CLog::Format("[Close] Id = %llu\n", player->steamID);
	WriteLog(ELogLevel::Warning, closeLog);

	_closeMsgObjectPool->ReturnObject(msg);
}

void CGameProcessor::WriteLog(const ELogLevel& level, const std::string& msg)
{
	CLog::WriteLog(ELogType::GameProcessor, level, msg);
}

CPlayer* CGameProcessor::AbsorbPlayer(CPlayer* from_new, CPlayer* to_old)
{
	//printf("Absorb %d -> %d\n", from_new.get(), to_old.get());
	if (!from_new || !to_old || from_new == to_old) return from_new;

	// 흡수전 방을 나간다.
	RoomManager->OutRoom(from_new);

	// 흡수할 플레이어의 소켓은 파괴시킨다.
	if (to_old->socket) {
		Close(to_old->socket);
		to_old->socket = nullptr;
	}

	PlayerManager->AbsorbPlayer(from_new, to_old);

	std::string closeLog = CLog::Format("[Close] IP = %s, Port = %d",
		inet_ntoa(to_old->addr.sin_addr),
		ntohs(to_old->addr.sin_port));
	WriteLog(ELogLevel::Warning, closeLog);

	return to_old;
}

void CGameProcessor::Send(CSocket* socket, FPacketStruct* packetStruct)
{
	char* buf = bufObjectPool->PopObject();
	packetStruct->Serialize(buf);
	Send(socket, buf, (int)packetStruct->GetSize());
	bufObjectPool->ReturnObject(buf);
}

void CGameProcessor::Send(CSocket* socket, const char* buf, const int& sendLen)
{
	ServerNetworkSystem->Send(socket, buf, sendLen);
}

void CGameProcessor::SendTo(const FSocketAddrIn& sockaddr, FPacketStruct* packetStruct)
{
	char* buf = bufObjectPool->PopObject();
	packetStruct->Serialize(buf);
	SendTo(sockaddr, buf, (int)packetStruct->GetSize());
	bufObjectPool->ReturnObject(buf);
}

void CGameProcessor::SendTo(const FSocketAddrIn& sockaddr, const char* buf, const int& sendLen)
{
	ServerNetworkSystem->SendTo(sockaddr, buf, sendLen);
}

void CGameProcessor::Close(CSocket* socket)
{
	ServerNetworkSystem->Close(socket);
}
