#pragma once
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/UtilityMacro.h"

template <typename Arg>
inline size_t GetSizeOf(Arg val) {
	return sizeof(val);
}
template <>
inline size_t GetSizeOf<FSerializableBuf>(FSerializableBuf val) {
	return val.GetSize();
}
template <typename T, size_t size>
inline size_t GetSizeOf(const T(&array)[size]) {
	size_t allSize = 0;
	for (size_t i = 0; i < size; ++i) {
		allSize += GetSizeOf(array[i]);
	}
	return allSize;
}
template <typename T, typename... Args>
inline size_t GetSizeOf(T first, Args... args) {
	return GetSizeOf(first) + GetSizeOf(args...);
}
template <typename T, size_t size, typename... Args>
inline size_t GetSizeOf(const T(&array)[size], Args... args) {
	return GetSizeOf(array) + GetSizeOf(args...);
}

template<typename T>
int __Serialize(char* buf, const int& len, T v) {
	return JHNETSerializer::TSerialize(buf + len, v);
}

template <typename T, size_t size>
int __Serialize(char* buf, const int& len, const T(&array)[size]) {
	return JHNETSerializer::TSerialize(buf + len, array);
}

template<typename T, typename... Args>
int __Serialize(char* buf, int len, T first, Args... args) {
	len += __Serialize(buf, len, first);
	len += __Serialize(buf, len, args...);
	return len;
}

template<typename T, size_t size, typename... Args>
int __Serialize(char* buf, int len, const T(&array)[size], Args... args) {
	len += __Serialize(buf, len, array);
	len += __Serialize(buf, len, args...);
	return len;
}

template <typename Arg>
void __Deserialize(char* buf, int* cursor, Arg& ref_target) {
	ref_target = JHNETSerializer::TDeserialize<Arg>(buf, cursor);
}

template <typename T, size_t size>
void __Deserialize(char* buf, int* cursor, T(&array)[size]) {
	JHNETSerializer::TDeserialize<T, size>(buf, cursor, array);
}

template<typename T, typename... Args>
void __Deserialize(char* buf, int* cursor, T& first, Args& ... args) {
	__Deserialize(buf, cursor, first);
	__Deserialize(buf, cursor, args...);
}

template<typename T, size_t size, typename... Args>
void __Deserialize(char* buf, int* cursor, T(&array)[size], Args& ... args) {
	__Deserialize(buf, cursor, array);
	__Deserialize(buf, cursor, args...);
}

// PACK_STRUCT
#define PACK_STRUCT(...)\
virtual size_t GetSize() {\
	return GetSizeOf(__VA_ARGS__) + FPacketStruct::GetSize();\
}\
virtual size_t Serialize(char* buf) {\
	int len = (int)FPacketStruct::Serialize(buf);\
	len = __Serialize(buf, len, __VA_ARGS__);\
	return len;\
}\
virtual void Deserialize(char* buf, int* cursor) {\
	FPacketStruct::Deserialize(buf, cursor);\
	__Deserialize(buf, cursor, __VA_ARGS__);\
}