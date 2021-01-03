#define INCL_EXTRA_HTON_FUNCTIONS
#include "Serializer.h"
#include <stdio.h>
#include "NetworkTool.h"
#include "NetworkModule/NetworkInfo.h"
using namespace NetworkTool;

int JHNETSerializer::EnumSerialize(char* outBuf, const EMessageType& type)
{
	int toIntType = (int)type;
	char buf[sizeof(int)];
	int intBufSize = IntSerialize(buf, toIntType);
	memcpy(outBuf, buf, intBufSize);
	return intBufSize;
}

int JHNETSerializer::SerializeWithEnum(const EMessageType& type, const char* inBuf, const int& bufLen, char* outBuf)
{
	int intBufSize = EnumSerialize(outBuf, type);
	if (bufLen != 0) memcpy(outBuf + intBufSize, inBuf, bufLen);
	return intBufSize + bufLen;
}

int JHNETSerializer::BoolSerialize(char * buf, const bool& data)
{
	buf[0] = data;
	return 1;
}

bool JHNETSerializer::BoolDeserialize(const char * buf, int* cursor)
{
	bool result;
	if (cursor == nullptr) result = static_cast<bool>(*buf);
	else {
		result = static_cast<bool>(*(buf + *cursor));
		*cursor += 1;
	}
	return result;
}

int JHNETSerializer::CharSerialize(char * buf, const char& data)
{
	buf[0] = data;
	return 1;
}


char JHNETSerializer::CharDeserialize(const char * buf, int* cursor)
{
	char result;
	if (cursor == nullptr) result = *buf;
	else {
		result = *(buf + *cursor);
		*cursor += 1;
	}
	return result;
}

EMessageType JHNETSerializer::EnumDeserialize(char * buf, int* cursor)
{
	int type = IntDeserialize(buf, cursor);
	return EMessageType(type);
}

int JHNETSerializer::IntSerialize(char * buf, const int32_t& val)
{
	Serialize((char*)&val, buf, sizeof(int32_t));
	return sizeof(int32_t);
}

int32_t JHNETSerializer::IntDeserialize(const char * buf, int* cursor)
{
	int32_t res = 0;
	if (cursor == nullptr) {
		Deserialize(buf, (char*)&res, sizeof(int32_t));
	}
	else {
		Deserialize(buf + *cursor, (char*)&res, sizeof(int32_t));
		*cursor += sizeof(int32_t);
	}
	return res;
}

int JHNETSerializer::UInt64Serialize(char * buf, const unsigned __int64 & val)
{
	Serialize((char*)&val, buf, sizeof(unsigned __int64));
	return sizeof(unsigned __int64);
}

unsigned __int64 JHNETSerializer::UInt64Deserialize(const char * buf, int* cursor)
{
	unsigned __int64 res = 0;
	if (cursor == nullptr) {
		Deserialize(buf, (char*)&res, sizeof(unsigned __int64));
	}
	else {
		Deserialize(buf + *cursor, (char*)&res, sizeof(unsigned __int64));
		*cursor += sizeof(unsigned __int64);
	}
	return res;
}

int JHNETSerializer::FloatSerialize(char * buf, const float& val)
{
	Serialize((char*)&val, buf, sizeof(float));
	return sizeof(float);
}

float JHNETSerializer::FloatDeserialize(const char * buf, int* cursor)
{
	float res = 0;
	if (cursor == nullptr) {
		Deserialize(buf, (char*)&res, sizeof(float));
	}
	else {
		Deserialize(buf + *cursor, (char*)&res, sizeof(float));
		*cursor += sizeof(float);
	}
	return res;
}

int JHNETSerializer::Vector3Serialize(char * buf, const FVector & val)
{
	FloatSerialize(buf, val.X);
	FloatSerialize(buf + sizeof(float), val.Y);
	FloatSerialize(buf + (sizeof(float) * 2), val.Z);
	return sizeof(float) * 3;
}

FVector JHNETSerializer::Vector3Deserialize(const char * buf, int* cursor)
{
	float x;
	float y;
	float z;
	if (cursor == nullptr) {
		x = FloatDeserialize(buf);
		y = FloatDeserialize(buf + sizeof(float));
		z = FloatDeserialize(buf + sizeof(float) * 2);
	}
	else {
		x = FloatDeserialize(buf, cursor);
		y = FloatDeserialize(buf, cursor);
		z = FloatDeserialize(buf, cursor);
	}
	FVector result(x, y, z);
	return result;
}

int JHNETSerializer::TransformSerialize(char* buf, const FTransform& val)
{
	FVector location = val.GetLocation();
	FVector rotation = val.GetRotation().Euler();
	FVector scale = val.GetScale3D();
	Vector3Serialize(buf, location);
	Vector3Serialize(buf + sizeof(float) * 3, rotation);
	Vector3Serialize(buf + sizeof(float) * 6, scale);
	return sizeof(float) * 9;
}

FTransform JHNETSerializer::TransformDeserialize(const char* buf, int* cursor /*= nullptr*/)
{
	FTransform newTransform;
	FVector location, rotation, scale;
	if (cursor == nullptr) {
		location = Vector3Deserialize(buf);
		rotation = Vector3Deserialize(buf + sizeof(float) * 3);
		scale = Vector3Deserialize(buf + sizeof(float) * 6);
	}
	else {
		location = Vector3Deserialize(buf, cursor);
		rotation = Vector3Deserialize(buf, cursor);
		scale = Vector3Deserialize(buf, cursor);
	}
	newTransform.SetLocation(location);
	newTransform.SetRotation(FQuat::MakeFromEuler(rotation));
	newTransform.SetScale3D(scale);
	return newTransform;
}

int JHNETSerializer::BufSerialize(char* buf, const FSerializableBuf& val)
{
	if (val.len >= SINGLE_PACKET_BUFSIZE || val.len < 0) return 0;
	int retval = IntSerialize(buf, val.len);
	memcpy(buf + retval, val.buf, val.len);
	return val.len + retval;
}

FSerializableBuf JHNETSerializer::BufDeserialize(const char * buf, int* cursor)
{
	int len = IntDeserialize(buf, cursor);
	if (len >= MAX_STRING_BUF || len < 0) {
		JHNET_LOG_ERROR("Wrong String...");
		*cursor = PACKET_BUFSIZE + 10000;
		return FSerializableBuf();
	}
	FSerializableBuf outData;
	outData.len = len;
	if (cursor == nullptr) {
		memcpy(outData.buf, buf, len);
	}
	else {
		memcpy(outData.buf, buf + *cursor, len);
		*cursor += len;
	}
	return outData;
}

int JHNETSerializer::StringSerialize(char * buf, std::string source)
{
	int len = source.length();
	if (len >= MAX_STRING_BUF) return 0;
	int retval = IntSerialize(buf, len);
	memcpy(buf + retval, source.c_str(), len);
	return len + retval;
}

void JHNETSerializer::Serialize(char* source, char* dest, int size)
{
	if (IsBigEndian()) {
		memcpy(dest, source, size);
	}
	else {
		for (int i = 0; i < size; ++i) {
			dest[i] = *(source + (size - 1 - i));
		}
	}
}

FString JHNETSerializer::FStringDeserialize(const char* buf, int* cursor)
{
	if (cursor == nullptr) {
		int len = 0;
		int stringLen = IntDeserialize(buf, &len);
		JHNET_CHECK(stringLen>=0 && stringLen < SINGLE_PACKET_BUFSIZE, FString());
		FString newString = BytesToString((uint8 *)((void*)(buf + len)), stringLen);
		return newString;
	}
	else {
		int stringLen = IntDeserialize(buf, cursor);
		JHNET_CHECK(stringLen >= 0 && stringLen < SINGLE_PACKET_BUFSIZE, FString());
		FString newString = BytesToString((uint8 *)((void*)(buf + *cursor)), stringLen);
		*cursor += stringLen;
		return newString;
	}
}

int JHNETSerializer::FStringSerialize(char* buf, const FString& source, int maxLen)
{
	int len = StringToBytes(source, (uint8 *)((void*)(buf + sizeof(int32_t))), maxLen);
	return IntSerialize(buf, len) + len;
}

void JHNETSerializer::Deserialize(const char * source, char * dest, int size)
{
	if (IsBigEndian()) {
		memcpy(dest, source, size);
	}
	else {
		for (int i = 0; i < size; ++i) {
			dest[i] = *(source + (size - 1 - i));
		}
	}
}

FSerializableBuf JHNETSerializer::FStringToBuf(const FString& val, int maxLen)
{
	FSerializableBuf newBuf;
	newBuf.len = FStringSerialize(newBuf.buf, val, maxLen);
	return newBuf;
}

FString JHNETSerializer::BufToFString(const FSerializableBuf& val)
{
	return FStringDeserialize(val.buf, nullptr);
}