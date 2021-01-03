#pragma once
#include "NetworkModule/Serializer.h"
#include "NetworkModule/UtilityMacro.h"

// RPC
// You need to use function's first line.
// ... is function args.
// RPC(NetBaseCP, AHACKEDCharacter, Move, ENetRpcType::Master, false, speed)
#define RPC(NETBASECP, CLASS_NAME, FUNCTION_NAME, E_NET_RPC_TYPE, IS_RELIABLE, ...)\
PP_MACRO_OVERLOAD_RPC(NETBASECP, CLASS_NAME, FUNCTION_NAME, &CLASS_NAME::FUNCTION_NAME, E_NET_RPC_TYPE, IS_RELIABLE, RPC, __VA_ARGS__)

// BindRPCFunction
// You muse use this in PostInitializeComponents or BeginPlay to register function.
// BindRPCFunction(NetBaseCP, AHACKEDCharacter, Move)
#define BindRPCFunction(NETBASECP, CLASS_NAME, FUNCTION_NAME)\
{\
	JHNET_CHECK(this);\
	JHNET_CHECK(NETBASECP);\
	auto rpcDelegate = NETBASECP->CreateBindableDelegateFunction(TEXT(#FUNCTION_NAME));\
	JHNET_CHECK(rpcDelegate);\
  	rpcDelegate->BindUObject(this, &CLASS_NAME::FUNCTION_NAME##_##);\
}

// RPC_FUNCTION
// You need to use this macro up of rpc function.
// (ClassName, FunctionName, args...)
// RPC_FUNCTION(AHACKEDCharacter, Move, int)
// Arg length available is 0~3
#define RPC_FUNCTION(CLASS_NAME, FUNCTION_NAME, ...)\
PP_MACRO_OVERLOAD_RPC_FUNCTION(CLASS_NAME, FUNCTION_NAME, RPC_FUNCTION, __VA_ARGS__)


#define MACRO_TO_CHAR(A)\
#A

template <typename FirstArg>
inline size_t GetSizeOfBuf(const char*) {
	return sizeof(FirstArg);
}

template <>
inline size_t GetSizeOfBuf<FString>(const char* buf) {
	return sizeof(int32_t) + JHNETSerializer::IntDeserialize(buf, nullptr);
}

template <>
inline size_t GetSizeOfBuf<FTransform>(const char* buf) {
	return sizeof(float) * 9;
}

template <typename T>
inline size_t GetSizeOfOrigin(T origin) {
	return sizeof(origin);
}

template <>
inline size_t GetSizeOfOrigin<FTransform>(FTransform origin) {
	return sizeof(float) * 9;
}

template <>
inline size_t GetSizeOfOrigin<FString>(FString origin) {
	int32 NumBytes = 0;
	const TCHAR* CharPos = *origin;

	while (*CharPos && NumBytes < PACKET_BUFSIZE)
	{
		CharPos++;
		++NumBytes;
	}
	return NumBytes + sizeof(int32_t); // FString은 Byte 앞에 int를 붙이므로 int를 추가로 적어줘야한다.
}

// RPC_FUNCTION
#define RPC_FUNCTION_0(CLASS_NAME, FUNCTION_NAME)\
bool CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
UFUNCTION()\
void FUNCTION_NAME##_##(int len, char* buf){\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = true;\
	if(len != 0){\
		JHNET_LOG(Error, "Argment Error!");\
		CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
		return;\
	}\
	auto func = std::bind(&CLASS_NAME::FUNCTION_NAME, this);\
	func();\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
}

#define RPC_FUNCTION_1(CLASS_NAME, FUNCTION_NAME, ARG1)\
bool CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
UFUNCTION()\
void FUNCTION_NAME##_##(int len, char* buf){\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = true;\
	if(len != GetSizeOfBuf<ARG1>(buf)){\
		JHNET_LOG(Error, "Argment Error!");\
		CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
		return;\
	}\
	auto func = std::bind(&CLASS_NAME::FUNCTION_NAME, this, std::placeholders::_1);\
	auto arg1 = JHNETSerializer::TDeserialize<ARG1>(buf, nullptr);\
	func(arg1);\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
}

#define RPC_FUNCTION_2(CLASS_NAME, FUNCTION_NAME, ARG1, ARG2)\
bool CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
UFUNCTION()\
void FUNCTION_NAME##_##(int len, char* buf){\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = true;\
	if(len != GetSizeOfBuf<ARG1>(buf) + \
				GetSizeOfBuf<ARG2>(buf + GetSizeOfBuf<ARG1>(buf))){\
		JHNET_LOG(Error, "Argment Error!");\
		CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
		return;\
	}\
	auto func = std::bind(&CLASS_NAME::FUNCTION_NAME, this, std::placeholders::_1, std::placeholders::_2);\
	auto arg1 = JHNETSerializer::TDeserialize<ARG1>(buf, nullptr);\
	auto arg2 = JHNETSerializer::TDeserialize<ARG2>(buf + GetSizeOfBuf<ARG1>(buf), nullptr);\
	func(arg1, arg2);\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
}

#define RPC_FUNCTION_3(CLASS_NAME, FUNCTION_NAME, ARG1, ARG2, ARG3)\
bool CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
UFUNCTION()\
void FUNCTION_NAME##_##(int len, char* buf){\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = true;\
	if(len != GetSizeOfBuf<ARG1>(buf) + \
				GetSizeOfBuf<ARG2>(buf + GetSizeOfBuf<ARG1>(buf)) + \
				GetSizeOfBuf<ARG3>(buf + GetSizeOfBuf<ARG1>(buf) + GetSizeOfBuf<ARG2>(buf + GetSizeOfBuf<ARG1>(buf)))){\
		JHNET_LOG(Error, "Argment Error!");\
		CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
		return;\
	}\
	auto func = std::bind(&CLASS_NAME::FUNCTION_NAME, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);\
	auto arg1 = JHNETSerializer::TDeserialize<ARG1>(buf, nullptr);\
	auto arg2 = JHNETSerializer::TDeserialize<ARG2>(buf + GetSizeOfBuf<ARG1>(buf), nullptr);\
	auto arg3 = JHNETSerializer::TDeserialize<ARG3>(buf + GetSizeOfBuf<ARG1>(buf) + GetSizeOfBuf<ARG2>(buf + GetSizeOfBuf<ARG1>(buf)), nullptr);\
	func(arg1, arg2, arg3);\
	CLASS_NAME##_##FUNCTION_NAME##_INLOOP = false;\
}

// RPC
#define RPC_0(NETBASECP, CLASS_NAME, FUNCTION_NAME, CLASS_AND_FUNCTION_NAME, E_NET_RPC_TYPE, IS_RELIABLE)\
{\
	if(NETBASECP && !CLASS_NAME##_##FUNCTION_NAME##_INLOOP){\
		NETBASECP->ExecuteNetFunc(\
			TEXT(#FUNCTION_NAME),\
			E_NET_RPC_TYPE, 0, nullptr, IS_RELIABLE, false);\
		if(E_NET_RPC_TYPE == ENetRPCType::MASTER && NETBASECP->IsSlave()) return;\
	}\
}

#define RPC_1(NETBASECP, CLASS_NAME, FUNCTION_NAME, CLASS_AND_FUNCTION_NAME, E_NET_RPC_TYPE, IS_RELIABLE, ARG1)\
{\
	if(NETBASECP && !CLASS_NAME##_##FUNCTION_NAME##_INLOOP){\
		char* __newBuf = NETBASECP->GetNetworkSystem()->bufObjectPool->PopObject();\
		int __len = JHNETSerializer::TSerialize(__newBuf, ARG1);\
		NETBASECP->ExecuteNetFunc(\
			TEXT(#FUNCTION_NAME),\
			E_NET_RPC_TYPE, __len, __newBuf, IS_RELIABLE, false);\
		NETBASECP->GetNetworkSystem()->bufObjectPool->ReturnObject(__newBuf);\
		if(E_NET_RPC_TYPE == ENetRPCType::MASTER && NETBASECP->IsSlave()) return;\
	}\
}

#define RPC_2(NETBASECP, CLASS_NAME, FUNCTION_NAME, CLASS_AND_FUNCTION_NAME, E_NET_RPC_TYPE, IS_RELIABLE, ARG1, ARG2)\
{\
	if(NETBASECP && !CLASS_NAME##_##FUNCTION_NAME##_INLOOP){\
		char* __newBuf = NETBASECP->GetNetworkSystem()->bufObjectPool->PopObject();\
		int __len = JHNETSerializer::TSerialize(__newBuf, ARG1);\
		__len += JHNETSerializer::TSerialize(__newBuf + GetSizeOfOrigin(ARG1), ARG2);\
		NETBASECP->ExecuteNetFunc(\
			TEXT(#FUNCTION_NAME),\
			E_NET_RPC_TYPE, __len, __newBuf, IS_RELIABLE, false);\
		NETBASECP->GetNetworkSystem()->bufObjectPool->ReturnObject(__newBuf);\
		if(E_NET_RPC_TYPE == ENetRPCType::MASTER && NETBASECP->IsSlave()) return;\
	}\
}

#define RPC_3(NETBASECP, CLASS_NAME, FUNCTION_NAME, CLASS_AND_FUNCTION_NAME, E_NET_RPC_TYPE, IS_RELIABLE, ARG1, ARG2, ARG3)\
{\
	if(NETBASECP && !CLASS_NAME##_##FUNCTION_NAME##_INLOOP){\
		char* __newBuf = NETBASECP->GetNetworkSystem()->bufObjectPool->PopObject();\
		int __len = JHNETSerializer::TSerialize(__newBuf, ARG1);\
		__len += JHNETSerializer::TSerialize(__newBuf + GetSizeOfOrigin(ARG1), ARG2);\
		__len += JHNETSerializer::TSerialize(__newBuf + GetSizeOfOrigin(ARG1) + GetSizeOfOrigin(ARG2), ARG3);\
		NETBASECP->ExecuteNetFunc(\
			TEXT(#FUNCTION_NAME),\
			E_NET_RPC_TYPE, __len, __newBuf, IS_RELIABLE, false); \
		NETBASECP->GetNetworkSystem()->bufObjectPool->ReturnObject(__newBuf); \
		if(E_NET_RPC_TYPE == ENetRPCType::MASTER && NETBASECP->IsSlave()) return;\
	}\
}