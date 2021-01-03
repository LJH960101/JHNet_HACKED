#pragma once
/*
	네트워크 전송을 위해 직렬화/역직렬화를 담당하는 static only 클래스
*/

#include "JHNETGameServer.h"
#include <type_traits>
#include "NetworkData.h"

typedef signed int INT32;
class FSerializableVector3;
struct FSerializableBuf;

namespace JHNETSerializer {
	int SerializeWithEnum(const EMessageType& type, const char* inBuf, const int& bufLen, char* outBuf);
	int EnumSerialize(char* buf, const EMessageType& type);
	EMessageType EnumDeserialize(char* buf, int* cursor = nullptr);
	int BoolSerialize(char* buf, const bool& data);
	bool BoolDeserialize(const char* buf, int* cursor = nullptr);
	int CharSerialize(char* buf, const char& data);
	char CharDeserialize(const char* buf, int* cursor = nullptr);
	int IntSerialize(char* buf, const INT32& val);
	INT32 IntDeserialize(char* buf, int* cursor = nullptr);
	int UInt64Serialize(char* buf, const UINT64& val);
	UINT64 UInt64Deserialize(char* buf, int* cursor = nullptr);
	int FloatSerialize(char* buf, const float& val);
	float FloatDeserialize(char* buf, int* cursor = nullptr);
	int Vector3Serialize(char* buf, const FSerializableVector3& val);
	FSerializableVector3 Vector3Deserialize(char* buf, int* cursor = nullptr);
	int BufSerialize(char* buf, const FSerializableBuf& val);
	FSerializableBuf BufDeserialize(char* buf, int* cursor = nullptr);
	int StringSerialize(char* buf, std::string source);
	void Serialize(char* source, char* dest, int size);
	void Deserialize(char* source, char* dest, int size);

	// Deserializer
	template<typename T>
	inline T TDeserialize(char* buf, int* cursor) {
		static_assert(std::is_default_constructible<T>::value,
			"Can't implemantable T.");
	}
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
	inline uint64_t TDeserialize<uint64_t>(char* buf, int* cursor) {
		return UInt64Deserialize(buf, cursor);
	}
	template<>
	inline float TDeserialize<float>(char* buf, int* cursor) {
		return FloatDeserialize(buf, cursor);
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
	inline int TSerialize(char* buf, const T& data) {
		static_assert(std::is_default_constructible<T>::value,
			"Can't implemantable T.");
	}
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
	inline int TSerialize<uint64_t>(char* buf, const uint64_t& data) {
		return UInt64Serialize(buf, data);
	}
	template<>
	inline int TSerialize<float>(char* buf, const float& data) {
		return FloatSerialize(buf, data);
	}
	template<>
	inline int TSerialize<std::string>(char* buf, const std::string& data) {
		return StringSerialize(buf, data);
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