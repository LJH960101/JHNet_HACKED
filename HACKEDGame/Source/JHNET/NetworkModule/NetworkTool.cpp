#include "NetworkTool.h"
#include "Serializer.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include <memory>

using namespace std;
using namespace NetworkTool;
using namespace JHNETSerializer;

int NetworkTool::Send(UObject* worldContext, SOCKET sock, const char * buf, int len, int flags)
{
	auto gameinstance = Cast<UJHNETGameInstance>(UGameplayStatics::GetGameInstance(worldContext));
	JHNET_CHECK(gameinstance && gameinstance->GetNetworkSystem(), 0);
	
	char* newBuf = gameinstance->GetNetworkSystem()->bufObjectPool->PopObject();
	int intLen = IntSerialize(newBuf, len);
	memcpy(newBuf + intLen, buf, len);

	int retval = SendWithoutLen(sock, newBuf, len + intLen, flags);

	gameinstance->GetNetworkSystem()->bufObjectPool->ReturnObject(newBuf);

	return retval;
}

int NetworkTool::SendWithoutLen(SOCKET sock, const char* buf, int len, int flags)
{
	return send(sock, buf, len, flags);
}

int NetworkTool::SendTo(UObject* worldContext, SOCKET sock, const char* buf, const int& len, sockaddr* addr, const int& addrLen)
{
	auto gameinstance = Cast<UJHNETGameInstance>(UGameplayStatics::GetGameInstance(worldContext));
	JHNET_CHECK(gameinstance && gameinstance->GetNetworkSystem(), 0);


	char* newBuf = gameinstance->GetNetworkSystem()->bufObjectPool->PopObject();
	int intLen = IntSerialize(newBuf, len);
	memcpy(newBuf + intLen, buf, len);

	int retval = SendToWithoutLen(sock, newBuf, len + intLen, addr, addrLen, 0);

	gameinstance->GetNetworkSystem()->bufObjectPool->ReturnObject(newBuf);

	return retval;
}

int NetworkTool::SendToWithoutLen(SOCKET sock, const char* buf, const int& len, sockaddr* addr, const int& addrLen, int flags)
{
	return sendto(sock, buf, len, flags, addr, addrLen);
}

bool NetworkTool::IsBigEndian()
{
	static bool onInit = false;
	static bool isBigEndian = false;

	if (!onInit) {
		onInit = true;
		unsigned int x = 0x76543210;
		char *c = (char*)&x;
		if (*c == 0x10)
		{
			isBigEndian = false;
		}
		else
		{
			isBigEndian = true;
		}
	}
	return isBigEndian;
}

FString NetworkTool::UINT64ToFString(const UINT64 val)
{
	return FString::Printf(TEXT("%llu"), val);
}

UINT64 NetworkTool::FStringToUINT64(const FString val)
{
	return FCString::Strtoui64(*val, NULL, 10);
}