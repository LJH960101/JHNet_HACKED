#pragma once
/*
	프로젝트 전체를 개괄하는 헤더
*/

#include <string.h>
#include <stdint.h>

#ifdef _WIN32 // Window Socket
#include <WinSock2.h>

#elif __linux__ // Linux Socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define DEBUG_RECV_MSG 0				// 디버깅 모드를 실시합니다. 서버 상태가 갱신되어 보이지 않습니다.
#define DEBUG_WARNING_LOG 0				// 경고 로그 이상 모두 출력합니다.
#define DEBUG_ERROR_LOG 0				// 에러 로그 이상 모두 출력합니다.
#define DEBUG_CRITICAL_LOG 0			// 크리티컬 로그만 출력합니다.
#define CUSTOM_DEBUG_MODE 0				// 서버 상태를 보지않고 직접 디버깅 모드를 합니다.

#define MAX_PLAYER 2					// 최대 플레이어수
#define TCP_SERVER_PORT "15470"			// TCP 포트
#define UDP_SERVER_PORT "15471"			// UDP 포트

#define PACKET_BUFSIZE 10000			// 총 패킷 크기
#define SINGLE_PACKET_BUFSIZE 5000		// 단일 메세지 최대 크기

typedef uint64_t UINT64;