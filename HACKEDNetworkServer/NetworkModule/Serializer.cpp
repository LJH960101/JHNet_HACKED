#include "Serializer.h"
#include "JHNETTool.h"
#include <stdio.h>
#include<string.h>
using namespace JHNETTool;

int JHNETSerializer::EnumSerialize(char * outBuf, const EMessageType& type)
{
	int toIntType = (int)type;
	char buf[sizeof(int)];
	int intBufSize = IntSerialize(buf, toIntType);
	memcpy(outBuf, buf, intBufSize);
	return intBufSize;
}

int JHNETSerializer::SerializeWithEnum(const EMessageType& type, const char* inBuf, const int& bufLen, char * outBuf)
{
	int intBufSize = EnumSerialize(outBuf, type);
	if(bufLen>=0 && bufLen <= PACKET_BUFSIZE) memcpy(outBuf+intBufSize, inBuf, bufLen);
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

int JHNETSerializer::IntSerialize(char * buf, const INT32& val)
{
	Serialize((char*)&val, buf, sizeof(INT32));
	return sizeof(INT32);
}

INT32 JHNETSerializer::IntDeserialize(char * buf, int* cursor)
{
	INT32 res = 0;
	if (cursor == nullptr) {
		Deserialize(buf, (char*)&res, sizeof(INT32));
	}
	else {
		Deserialize(buf + *cursor, (char*)&res, sizeof(INT32));
		*cursor += (int)sizeof(INT32);
	}
	return res;
}

int JHNETSerializer::UInt64Serialize(char * buf, const UINT64 & val)
{
	Serialize((char*)&val, buf, sizeof(UINT64));
	return sizeof(UINT64);
}

UINT64 JHNETSerializer::UInt64Deserialize(char * buf, int* cursor)
{
	UINT64 res = 0;
	if (cursor == nullptr) {
		Deserialize(buf, (char*)&res, sizeof(UINT64));
	}
	else {
		Deserialize(buf + *cursor, (char*)&res, sizeof(UINT64));
		*cursor += (int)sizeof(UINT64);
	}
	return res;
}

int JHNETSerializer::FloatSerialize(char * buf, const float& val)
{
	Serialize((char*)&val, buf, sizeof(float));
	return sizeof(float);
}

float JHNETSerializer::FloatDeserialize(char * buf, int* cursor)
{
	float res = 0;
	if (cursor == nullptr) {
		Deserialize(buf, (char*)&res, sizeof(float));
	}
	else {
		Deserialize(buf + *cursor, (char*)&res, sizeof(float));
		*cursor += (int)sizeof(float);
	}
	return res;
}

int JHNETSerializer::Vector3Serialize(char * buf, const FSerializableVector3 & val)
{
	FloatSerialize(buf, val.x);
	FloatSerialize(buf + sizeof(float), val.y);
	FloatSerialize(buf + (sizeof(float) * 2), val.z);
	return (int)sizeof(float) * 3;
}

FSerializableVector3 JHNETSerializer::Vector3Deserialize(char * buf, int* cursor)
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
	FSerializableVector3 result(x, y, z);
	return result;
}

int JHNETSerializer::BufSerialize(char* buf, const FSerializableBuf& val)
{
	if (val.len >= SINGLE_PACKET_BUFSIZE || val.len < 0) return 0;
	int retval = IntSerialize(buf, val.len);
	memcpy(buf + retval, val.buf, val.len);
	return val.len + retval;
}

FSerializableBuf JHNETSerializer::BufDeserialize(char * buf, int* cursor)
{
	int len = IntDeserialize(buf, cursor);
	if (len >= SINGLE_PACKET_BUFSIZE || len < 0) throw "String Deserialize : Too large buf size.";
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
	int len = static_cast<int>(source.length());
	if (len > SINGLE_PACKET_BUFSIZE || len < 0) return 0;
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

void JHNETSerializer::Deserialize(char * source, char * dest, int size)
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