#pragma once

struct FGameProcessorMessage {
	enum class EMessageType {
		ACCEPT,
		TCP,
		UDP,
		CLOSE
	};
	EMessageType type;
};

struct FAcceptMessage : public FGameProcessorMessage {
	FAcceptMessage() {
		type = EMessageType::ACCEPT;
	}
	CSocket* socket;
	FSocketAddrIn clientAddr;
};

struct FCloseMessage : public FGameProcessorMessage {
	FCloseMessage() {
		type = EMessageType::CLOSE;
	}
	CSocket* socket;
};

struct FRecvMessage : public FGameProcessorMessage {
	FRecvMessage() {}
	char buf[PACKET_BUFSIZE];
	int len;
};

struct FTCPMessage : public FRecvMessage {
	FTCPMessage() {
		type = EMessageType::TCP;
	}
	CSocket* socket;
};

struct FUDPMessage : public FRecvMessage {
	FUDPMessage() {
		type = EMessageType::UDP;
	}
	FSocketAddrIn udpAddr;
};