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
void __Serialize(char* buf, int& len, T v) {
	len += JHNETSerializer::TSerialize(buf + len, v);
}

template <typename T, size_t size>
void __Serialize(char* buf, int& len, const T(&array)[size]) {
	len += JHNETSerializer::TSerialize(buf + len, array);
}

template<typename T, typename... Args>
void __Serialize(char* buf, int& len, T first, Args... args) {
	__Serialize(buf, len, first);
	__Serialize(buf, len, args...);
}

template<typename T, size_t size, typename... Args>
void __Serialize(char* buf, int& len, const T(&array)[size], Args... args) {
	__Serialize(buf, len, array);
	__Serialize(buf, len, args...);
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
void __Deserialize(char* buf, int* cursor, T& first, Args & ... args) {
	__Deserialize(buf, cursor, first);
	__Deserialize(buf, cursor, args...);
}

template<typename T, size_t size, typename... Args>
void __Deserialize(char* buf, int* cursor, T(&array)[size], Args & ... args) {
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
	__Serialize(buf, len, __VA_ARGS__);\
	return len;\
}\
virtual void Deserialize(char* buf, int* cursor) {\
	FPacketStruct::Deserialize(buf, cursor);\
	__Deserialize(buf, cursor, __VA_ARGS__);\
}


/*
#define PACK_STRUCT(...)\
PP_MACRO_OVERLOAD_PACK_STRUCT(PACK_STRUCT, __VA_ARGS__)

#define PACK_STRUCT_0() NOT_USED

#define PACK_STRUCT_1(ARG1)\
virtual size_t GetSize() {\
	return GetSizeOf(ARG1) + FPacketStruct::GetSize();\
}\
virtual size_t Serialize(char* buf) {\
	int len = (int)FPacketStruct::Serialize(buf);\
	len = __Serialize(buf, len, ARG1);\
	return len;\
}\
virtual void Deserialize(char* buf, int* cursor) {\
	FPacketStruct::Deserialize(buf, cursor);\
	_Deserialize(buf, cursor, ARG1);\
}

#define PACK_STRUCT_2(ARG1, ARG2)\
virtual size_t GetSize() {\
	return GetSizeOf(ARG1) + GetSizeOf(ARG2) + FPacketStruct::GetSize();\
}\
virtual size_t Serialize(char* buf) {\
	size_t len = FPacketStruct::Serialize(buf);\
	len += JHNETSerializer::TSerialize(buf + len, ARG1);\
	len += JHNETSerializer::TSerialize(buf + len, ARG2);\
	return len;\
}\
virtual void Deserialize(char* buf, int* cursor) {\
	FPacketStruct::Deserialize(buf, cursor);\
	_Deserialize(buf, cursor, ARG1);\
	_Deserialize(buf, cursor, ARG2);\
}

#define PACK_STRUCT_3(ARG1, ARG2, ARG3)\
virtual size_t GetSize() {\
	return GetSizeOf(ARG1) + GetSizeOf(ARG2) + GetSizeOf(ARG3) + FPacketStruct::GetSize();\
}\
virtual size_t Serialize(char* buf) {\
	size_t len = FPacketStruct::Serialize(buf);\
	len += JHNETSerializer::TSerialize(buf + len, ARG1);\
	len += JHNETSerializer::TSerialize(buf + len, ARG2);\
	len += JHNETSerializer::TSerialize(buf + len, ARG3);\
	return len;\
}\
virtual void Deserialize(char* buf, int* cursor) {\
	FPacketStruct::Deserialize(buf, cursor);\
	_Deserialize(buf, cursor, ARG1);\
	_Deserialize(buf, cursor, ARG2);\
	_Deserialize(buf, cursor, ARG3);\
}

#define PACK_STRUCT_4(ARG1, ARG2, ARG3, ARG4)\
virtual size_t GetSize() {\
	return GetSizeOf(ARG1) + GetSizeOf(ARG2) + GetSizeOf(ARG3) + GetSizeOf(ARG4) + FPacketStruct::GetSize();\
}\
virtual size_t Serialize(char* buf) {\
	size_t len = FPacketStruct::Serialize(buf);\
	len += JHNETSerializer::TSerialize(buf + len, ARG1);\
	len += JHNETSerializer::TSerialize(buf + len, ARG2);\
	len += JHNETSerializer::TSerialize(buf + len, ARG3);\
	len += JHNETSerializer::TSerialize(buf + len, ARG4);\
	return len;\
}\
virtual void Deserialize(char* buf, int* cursor) {\
	FPacketStruct::Deserialize(buf, cursor);\
	_Deserialize(buf, cursor, ARG1);\
	_Deserialize(buf, cursor, ARG2);\
	_Deserialize(buf, cursor, ARG3);\
	_Deserialize(buf, cursor, ARG4);\
}
*/