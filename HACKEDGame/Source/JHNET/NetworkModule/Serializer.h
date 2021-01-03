#pragma once
#include "NetworkModule/NetworkInfo.h"
#include "NetworkData.h"
#include <stdint.h>
#include "JHNET.h"
#define DLL_EXPORT __declspec(dllexport)

namespace JHNETSerializer {
	JHNET_API int SerializeWithEnum(const EMessageType& type, const char* inBuf, const int& bufLen, char* outBuf);
	JHNET_API int EnumSerialize(char* buf, const EMessageType& type);
	JHNET_API EMessageType EnumDeserialize(char* buf, int* cursor = nullptr);
	JHNET_API int BoolSerialize(char* buf, const bool& data);
	JHNET_API bool BoolDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int CharSerialize(char* buf, const char& data);
	JHNET_API char CharDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int IntSerialize(char* buf, const int32_t& val);
	JHNET_API int32_t IntDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int UInt64Serialize(char* buf, const unsigned __int64& val);
	JHNET_API unsigned __int64 UInt64Deserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int FloatSerialize(char* buf, const float& val);
	JHNET_API float FloatDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int Vector3Serialize(char* buf, const FVector& val);
	JHNET_API FVector Vector3Deserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int TransformSerialize(char* buf, const FTransform& val);
	JHNET_API FTransform TransformDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int BufSerialize(char* buf, const FSerializableBuf& val);
	JHNET_API FSerializableBuf BufDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int StringSerialize(char* buf, std::string source);
	JHNET_API FString FStringDeserialize(const char* buf, int* cursor = nullptr);
	JHNET_API int FStringSerialize(char* buf, const FString& source, int maxLen = SINGLE_PACKET_BUFSIZE);
	JHNET_API void Serialize(char* source, char* dest, int size);
	JHNET_API void Deserialize(const char* source, char* dest, int size);
	JHNET_API FSerializableBuf FStringToBuf(const FString& val, int maxLen = SINGLE_PACKET_BUFSIZE);
	JHNET_API FString BufToFString(const FSerializableBuf& val);

	// Deserializer
	template<typename T>
	inline T TDeserialize(char* buf, int* cursor);
	template<>
	inline int TDeserialize<int>(char* buf, int* cursor) {
		return IntDeserialize(buf, cursor);
	}
	template<>
	inline EMessageType TDeserialize<EMessageType>(char* buf, int* cursor) {
		return EnumDeserialize(buf, cursor);
	}
	template<>
	inline bool TDeserialize<bool>(char* buf, int* cursor) {
		return BoolDeserialize(buf, cursor);
	}
	template<>
	inline char TDeserialize<char>(char* buf, int* cursor) {
		return CharDeserialize(buf, cursor);
	}
	template<>
	inline unsigned __int64 TDeserialize<unsigned __int64>(char* buf, int* cursor) {
		return UInt64Deserialize(buf, cursor);
	}
	template<>
	inline float TDeserialize<float>(char* buf, int* cursor) {
		return FloatDeserialize(buf, cursor);
	}
	template<>
	inline FVector TDeserialize<FVector>(char* buf, int* cursor) {
		return Vector3Deserialize(buf, cursor);
	}
	template<>
	inline FTransform TDeserialize<FTransform>(char* buf, int* cursor) {
		return TransformDeserialize(buf, cursor);
	}
	template<>
	inline FString TDeserialize<FString>(char* buf, int* cursor) {
		return FStringDeserialize(buf, cursor);
	}
	template<>
	inline FSerializableBuf TDeserialize<FSerializableBuf>(char* buf, int* cursor) {
		return BufDeserialize(buf, cursor);
	}
	template <typename T, size_t size>
	inline void TDeserialize(char* buf, int* cursor, T(&array)[size]) {
		for (int i = 0; i < size; ++i) {
			array[i] = TDeserialize<T>(buf, cursor);
		}
	}

	// Serializer
	template<typename T>
	inline int TSerialize(char* buf, const T& data);
	template<>
	inline int TSerialize<int>(char* buf, const int& data) {
		return IntSerialize(buf, data);
	}
	template<>
	inline int TSerialize<EMessageType>(char* buf, const EMessageType& data) {
		return EnumSerialize(buf, data);
	}
	template<>
	inline int TSerialize<bool>(char* buf, const bool& data) {
		return BoolSerialize(buf, data);
	}
	template<>
	inline int TSerialize<char>(char* buf, const char& data) {
		return CharSerialize(buf, data);
	}
	template<>
	inline int TSerialize<unsigned __int64>(char* buf, const unsigned __int64& data) {
		return UInt64Serialize(buf, data);
	}
	template<>
	inline int TSerialize<float>(char* buf, const float& data) {
		return FloatSerialize(buf, data);
	}
	template<>
	inline int TSerialize<FVector>(char* buf, const FVector& data) {
		return Vector3Serialize(buf, data);
	}
	template<>
	inline int TSerialize<FTransform>(char* buf, const FTransform& data) {
		return TransformSerialize(buf, data);
	}
	template<>
	inline int TSerialize<std::string>(char* buf, const std::string& data) {
		return StringSerialize(buf, data);
	}
	template<>
	inline int TSerialize<FString>(char* buf, const FString& data) {
		return FStringSerialize(buf, data, SINGLE_PACKET_BUFSIZE);
	}
	template<>
	inline int TSerialize<FSerializableBuf>(char* buf, const FSerializableBuf& data) {
		return BufSerialize(buf, data);
	}
	template <typename T, size_t size>
	inline int TSerialize(char* buf, const T(&array)[size]) {
		int allSize = 0;
		for (int i = 0; i < size; ++i) {
			allSize += TSerialize(buf + allSize, array[i]);
		}
		return allSize;
	}
}