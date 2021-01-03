#include "Log.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include "JHNETGameServer.h"
#include "NetworkModule/MPSCQueue.h"

const bool DISABLE_LOG = false;

using namespace std;

CLog* CLog::instance = nullptr;

CLog::CLog()
{
	_logQueue = new CMessageQueue<FLog>();
	_currentQueue = _logQueue->CreateQueue();
	_bOnWrite = false;
}

CLog::~CLog()
{
	Shutdown();
	if (_logQueue) delete _logQueue;
	if (_currentQueue) delete _currentQueue;
}

void CLog::WriteLog(const ELogType& logType, const ELogLevel& logLevel, const string& format)
{
	if (DISABLE_LOG) return;

	CLog* logSystem = GetInstance();

	// Calc Time
	char buf[80];

	time_t t = time(0);
#ifdef _WIN32 // WINDOW
	tm time;
	localtime_s(&time, &t);
	sprintf_s(buf, "(%d/%d %d:%d:%d)", (time.tm_mon + 1), time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
#elif __linux__ // LINUX
	tm* time = nullptr;
	time = localtime(&t);
	sprintf(buf, "(%d/%d %d:%d:%d)", (time->tm_mon + 1), time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
#endif

	// Create LogText
	string logText = buf + _LogLevelToString(logLevel) + string(" : ") + _LogTypeToString(logType) + string(", ") + format + string("\n");

	// Push to queue
	FLog newLog;
	newLog.type = logType;
	newLog.level = logLevel;
	newLog.log = logText;

	logSystem->_logQueue->Enqueue(newLog);

	// Do _WriteLog when not on write
	if (!logSystem->_bOnWrite) {
		logSystem->_bOnWrite = true;
		logSystem->_writeLogThread = new thread(&CLog::_WriteLog, logSystem);
	}
}

void CLog::WriteLogS(const string& format)
{
	if (DISABLE_LOG) return;

	WriteLog(ELogType::LogTemp, ELogLevel::Warning, format);
}

std::string CLog::Format(const char * format, ...)
{
	if (DISABLE_LOG) return std::string("");

	va_list args;
	va_start(args, format);
#ifndef _MSC_VER
	size_t size = std::snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	std::vsnprintf(buf.get(), size, format, args);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
#else
	int size = _vscprintf(format, args);
	std::string result(++size, 0);
	vsnprintf_s((char*)result.data(), size, _TRUNCATE, format, args);
	return result;
#endif
	va_end(args);
	return std::string("???");
}

void CLog::Join()
{
	CLog* logSystem = GetInstance();

	if (logSystem->_writeLogThread == nullptr) return;
	logSystem->_writeLogThread->join();
}

void CLog::Shutdown()
{
	CLog* logSystem = GetInstance();

	logSystem->_bOnWrite = false;

	if (logSystem->_writeLogThread) {
		logSystem->Join();
		delete logSystem->_writeLogThread;
	}
}

string CLog::_LogTypeToString(const ELogType& logType)
{
	switch (logType)
	{
	case ELogType::LogTemp:
		return "LogTemp";
	case ELogType::LogSystem:
		return "LogSystem";
	case ELogType::NetworkManager:
		return "NetworkManager";
	case ELogType::ServerNetworkSystem:
		return "ServerNetworkSystem";
	case ELogType::RoomManager:
		return "RoomManager";
	case ELogType::PlayerManager:
		return "PlayerManager";
	case ELogType::TCPReceiveProcessor:
		return "TCPReceiveProcessor";
	case ELogType::IOCPServer:
		return "IOCPServer";
	case ELogType::UDPReceiveProcessor:
		return "ReceiveProcessor";
	case ELogType::Socket:
		return "Socket";
	case ELogType::Room:
		return "Room";
	case ELogType::EpollServer:
		return "EpollServer";
	case ELogType::GameProcessor:
		return "GameProcessor";
	default:
		return "UnkownLogType";
	}
}

string CLog::_LogLevelToString(const ELogLevel & logLevel)
{
	switch (logLevel)
	{
	case ELogLevel::Warning:
		return "Warning";
	case ELogLevel::Error:
		return "Error";
	case ELogLevel::Critical:
		return "Critical";
	default:
		return "UnknownLevel";
	}
}

CLog* CLog::GetInstance()
{
	if (instance == nullptr) instance = new CLog();
	return instance;
}

void CLog::_WriteLog()
{
	if (DISABLE_LOG) return;

	std::chrono::milliseconds sleepDuration(10);
	FLog currentLog;
	while (_bOnWrite) {
		// Pop log
		_logQueue->UpdateQueue(_currentQueue);
		while (!_currentQueue->empty()) {
			currentLog = _currentQueue->front();
			_currentQueue->pop();

#if DEBUG_WARNING_LOG
			if ((int)currentLog.level >= (int)ELogLevel::Warning) std::cout << currentLog.log << std::endl;
#elif DEBUG_ERROR_LOG
			if ((int)currentLog.level >= (int)ELogLevel::Error) std::cout << currentLog.log << std::endl;
#elif DEBUG_CRITICAL_LOG
			if ((int)currentLog.level >= (int)ELogLevel::Critical) std::cout << currentLog.log << std::endl;
#endif

			// Create LogFile Name
			string fileName1 = _LogTypeToString(currentLog.type);
			string fileName2 = LOG_INTERGRATED;
			fileName1.append(LOG_FILE_TYPE);
			fileName2.append(LOG_FILE_TYPE);

			// Write Log
			ofstream output1(fileName1, ios::app);
			ofstream output2(fileName2, ios::app);
			output1 << currentLog.log;
			output2 << currentLog.log;
			output1.close();
			output2.close();
		}
		std::this_thread::sleep_for(sleepDuration);
	}
}
