#pragma once
/*
	������Ʈ ��ü�� �����ϴ� ���
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

#define DEBUG_RECV_MSG 0				// ����� ��带 �ǽ��մϴ�. ���� ���°� ���ŵǾ� ������ �ʽ��ϴ�.
#define DEBUG_WARNING_LOG 0				// ��� �α� �̻� ��� ����մϴ�.
#define DEBUG_ERROR_LOG 0				// ���� �α� �̻� ��� ����մϴ�.
#define DEBUG_CRITICAL_LOG 0			// ũ��Ƽ�� �α׸� ����մϴ�.
#define CUSTOM_DEBUG_MODE 0				// ���� ���¸� �����ʰ� ���� ����� ��带 �մϴ�.

#define MAX_PLAYER 2					// �ִ� �÷��̾��
#define TCP_SERVER_PORT "15470"			// TCP ��Ʈ
#define UDP_SERVER_PORT "15471"			// UDP ��Ʈ

#define PACKET_BUFSIZE 10000			// �� ��Ŷ ũ��
#define SINGLE_PACKET_BUFSIZE 5000		// ���� �޼��� �ִ� ũ��

typedef uint64_t UINT64;