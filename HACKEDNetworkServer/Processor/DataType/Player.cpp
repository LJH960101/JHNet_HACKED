#include "Player.h"
#include "Processor/Manager/PlayerManager.h"
#include "Processor/GameProcessor.h"

CPlayer::CPlayer()
{
	delayedTCPDataLen = 0;
	delayedUDPDataLen = 0;
}

CPlayer::~CPlayer()
{
	if (socket) {
		GameProcessor->Close(socket);
		socket = nullptr;
	}
	GameProcessor->bufObjectPool->ReturnObject(delayedTCPData);
	GameProcessor->bufObjectPool->ReturnObject(delayedUDPData);
}

void CPlayer::Init()
{
	steamID = 0;
	lastPingTime = std::chrono::system_clock::now();
	lastPongTime = std::chrono::system_clock::now();
	state = LOBBY;
	bHaveUDP = false;
	delayedTCPDataLen = 0;
	delayedUDPDataLen = 0;
	if(!delayedTCPData)	delayedTCPData = GameProcessor->bufObjectPool->PopObject();
	if(!delayedUDPData) delayedUDPData = GameProcessor->bufObjectPool->PopObject();
}

int CPlayer::PopDelayData(char* buf, int& ref_len, bool isTCP)
{
	if (ref_len < 0 || ref_len > PACKET_BUFSIZE) return 0;

	char* targetDelayedData;
	int* targetDelayedLen;

	if (isTCP) {
		targetDelayedData = delayedTCPData;
		targetDelayedLen = &delayedTCPDataLen;
	}
	else {
		targetDelayedData = delayedUDPData;
		targetDelayedLen = &delayedUDPDataLen;
	}

	if (*targetDelayedLen == 0) return 0;

	// 꺼내고 없애주기만 되는 경우?
	if (*targetDelayedLen + ref_len < PACKET_BUFSIZE) {
		// 기존 내용을 밀어낸다.
		memmove(buf + *targetDelayedLen, buf, ref_len);
		// 앞에다가 딜레이된 내용을 붙여준다.
		memcpy(buf, targetDelayedData, *targetDelayedLen);

		// 이전 내용은 없애준다.
		*targetDelayedLen = 0;

		// 새로워진 길이를 리턴.
		ref_len = ref_len + *targetDelayedLen;
		return ref_len + *targetDelayedLen;
	}
	// 합치면 버퍼보다 많을 경우
	else {
		char newBuf[PACKET_BUFSIZE];

		// 수신받은 오버랩에서 짤리는 전 부분 (자르는 길이)
		int preSplitLen = PACKET_BUFSIZE - *targetDelayedLen;
		// 수신받은 오버랩에서 짤리는 후 부분 (잘린 후 길이)
		int postSplitLen = (*targetDelayedLen + ref_len) - PACKET_BUFSIZE;

		// 딜레이된 버퍼를 덮는다.
		memcpy(newBuf, targetDelayedData, *targetDelayedLen);
		// 기존 버퍼를 최대 패킷 사이즈 만큼 만든다.
		memcpy(newBuf + *targetDelayedLen,
			buf,
			preSplitLen // 최대 패킷을 넘기지 않도록.
		);

		// 딜레이 오버랩을 다시 맞춰준다.
		memcpy(targetDelayedData,
			buf + preSplitLen, // 잘린 부분 이후부터
			postSplitLen // 잘린 부분의 길이 까지
		);

		// newBuf로 버퍼를 다시 맞춰준다.
		memcpy(buf, newBuf, PACKET_BUFSIZE);
		ref_len = PACKET_BUFSIZE;

		*targetDelayedLen = postSplitLen;

		return PACKET_BUFSIZE;
	}
}

void CPlayer::PushDelayData(char* buf, const int& cursor, const int& recvLen, bool isTCP)
{
	char* targetDelayedData;
	int* targetDelayedLen;

	if (isTCP) {
		targetDelayedData = delayedTCPData;
		targetDelayedLen = &delayedTCPDataLen;
	}
	else {
		targetDelayedData = delayedUDPData;
		targetDelayedLen = &delayedUDPDataLen;
	}

	// 이미 딜레이가 존재한다면 새로운 버퍼에다가 앞에는 남은 데이터 뒤에는 이미 존재하는 딜레이를 붙여 다시 만든다.
	if (*targetDelayedLen > 0) {
		// 잘릴 데이터 만큼 앞자리를 비워준다.
		memmove(targetDelayedData + (recvLen - cursor), targetDelayedData, recvLen - cursor);

		// 앞에다가 잘린 데이터를 넣어준다.
		memcpy(targetDelayedData, buf + cursor, recvLen - cursor);

		// 딜레이 오버랩을 교체해준다.
		*targetDelayedLen = recvLen - cursor + *targetDelayedLen;
	}
	// 아니라면 남는것만 딜레이에 넣는다.
	else {
		memcpy(targetDelayedData, buf + cursor, recvLen - cursor);
		*targetDelayedLen = recvLen - cursor;
	}
}

bool CPlayer::SetUDPAddr(const FSocketAddrIn& newAddr)
{
	this->udpAddr = newAddr;
	bHaveUDP = true;

	if (GameProcessor->PlayerManager->IsContain(this)) return true;
	else return false;
}

void CPlayer::RemoveUDPAddr()
{
	bHaveUDP = false;
}

void CPlayer::SetGameProcessor(class CGameProcessor* gameProcessor)
{
	GameProcessor = gameProcessor;
}
