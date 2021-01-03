# HACKEDHeartbeatServer

블랙말랑카우팀 HACKED Heartbeat Server 입니다.

본 서버가 살아있는지 감시하고, 본 서버가 죽었다면 재 실행 시키는 서버 입니다.

info.txt에 다음과 같은 설정을 해야합니다.

```

ServerPath=서버 파일 경로
IP=IP 주소
TCP_SERVER_PORT=TCP서버 포트
UDP_SERVER_PORT=UDP서버 포트
HEARTBEAT_MESSAGE_TYPE=하트비트 메세지 타입

```