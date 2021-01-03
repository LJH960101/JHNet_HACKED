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

	// ������ �����ֱ⸸ �Ǵ� ���?
	if (*targetDelayedLen + ref_len < PACKET_BUFSIZE) {
		// ���� ������ �о��.
		memmove(buf + *targetDelayedLen, buf, ref_len);
		// �տ��ٰ� �����̵� ������ �ٿ��ش�.
		memcpy(buf, targetDelayedData, *targetDelayedLen);

		// ���� ������ �����ش�.
		*targetDelayedLen = 0;

		// ���ο��� ���̸� ����.
		ref_len = ref_len + *targetDelayedLen;
		return ref_len + *targetDelayedLen;
	}
	// ��ġ�� ���ۺ��� ���� ���
	else {
		char newBuf[PACKET_BUFSIZE];

		// ���Ź��� ���������� ©���� �� �κ� (�ڸ��� ����)
		int preSplitLen = PACKET_BUFSIZE - *targetDelayedLen;
		// ���Ź��� ���������� ©���� �� �κ� (�߸� �� ����)
		int postSplitLen = (*targetDelayedLen + ref_len) - PACKET_BUFSIZE;

		// �����̵� ���۸� ���´�.
		memcpy(newBuf, targetDelayedData, *targetDelayedLen);
		// ���� ���۸� �ִ� ��Ŷ ������ ��ŭ �����.
		memcpy(newBuf + *targetDelayedLen,
			buf,
			preSplitLen // �ִ� ��Ŷ�� �ѱ��� �ʵ���.
		);

		// ������ �������� �ٽ� �����ش�.
		memcpy(targetDelayedData,
			buf + preSplitLen, // �߸� �κ� ���ĺ���
			postSplitLen // �߸� �κ��� ���� ����
		);

		// newBuf�� ���۸� �ٽ� �����ش�.
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

	// �̹� �����̰� �����Ѵٸ� ���ο� ���ۿ��ٰ� �տ��� ���� ������ �ڿ��� �̹� �����ϴ� �����̸� �ٿ� �ٽ� �����.
	if (*targetDelayedLen > 0) {
		// �߸� ������ ��ŭ ���ڸ��� ����ش�.
		memmove(targetDelayedData + (recvLen - cursor), targetDelayedData, recvLen - cursor);

		// �տ��ٰ� �߸� �����͸� �־��ش�.
		memcpy(targetDelayedData, buf + cursor, recvLen - cursor);

		// ������ �������� ��ü���ش�.
		*targetDelayedLen = recvLen - cursor + *targetDelayedLen;
	}
	// �ƴ϶�� ���°͸� �����̿� �ִ´�.
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
