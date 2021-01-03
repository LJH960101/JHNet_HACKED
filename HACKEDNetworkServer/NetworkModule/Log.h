#pragma once
/*
	Thread safe하게 로그를 기록하는 클래스
*/

#include <string>
#include <queue>
#include <thread>
#include "NetworkModule/MessageQueue.h"

#define LOG_INTERGRATED "LogIntergrated"
#define LOG_FILE_TYPE ".log"

enum class ELogLevel : int
{
	Warning = 0,
	Error,
	Critical
};
enum class ELogType {
	LogTemp,
	LogSystem,
	NetworkManager,
	ServerNetworkSystem,
	RoomManager,
	PlayerManager,
	IOCPServer,
	EpollServer,
	TCPReceiveProcessor,
	UDPReceiveProcessor,
	Socket,
	Room,
	GameProcessor
};

struct FLog
{
	ELogType type;
	ELogLevel level;
	std::string log;
};

class CLog
{
	// FILED
private:
	bool _bOnWrite;
	std::thread* _writeLogThread;
	CMessageQueue<FLog>* _logQueue;
	CMessageQueue<FLog>::QueueType _currentQueue;
	static std::string _LogTypeToString(const ELogType& logType);
	static std::string _LogLevelToString(const ELogLevel& logLevel);

	// METHOD
public:
	// Join While thread end.
	static void Join();
	// Stop Log Threads.
	static void Shutdown();
	// Push Log to queue by many args.
	static void WriteLog(const ELogType& logType, const ELogLevel& logLevel, const std::string& format);
	// Push Log to queue by simple string.
	static void WriteLogS(const std::string& format);
	static std::string Format(const char* format, ...);

private:
	static CLog* instance;

	// Real Write Log
	CLog();
	~CLog();
	void _WriteLog();
	static CLog* GetInstance();
};