// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyNetworkProcessor.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "Lobby/LobbyManager.h"
#include "Lobby/Widget/WGFriendInfo.h"
#include "Blueprint/UserWidget.h"
#include "NetworkModule/Serializer.h"
#include "NetworkModule/NetworkTool.h"
#include "NetworkModule/ConfigParser/ConfigParser.h"

using namespace NetworkTool;
using namespace JHNETSerializer;

// Sets default values
ALobbyNetworkProcessor::ALobbyNetworkProcessor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALobbyNetworkProcessor::BeginPlay()
{
	Super::BeginPlay();

	if (!fadeScreen) fadeScreen = CreateWidget<UWGFade>(GetWorld(), fadeScreenClass);
	if (!fadeScreen->IsInViewport()) fadeScreen->AddToViewport(fadeZIndex);
	if (fadeScreen) fadeScreen->FadeIn(1.0f);

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyManager::StaticClass(), actors);
	if (actors.Num() == 0) {
		JHNET_LOG_SCREEN("LobbyManager must be placed in level!!!!");
		JHNET_LOG(Error, "LobbyManager must be placed in level!!!!");
	}
	else if (actors.Num() > 1) {
		JHNET_LOG_SCREEN("Only one of LobbyManager must be placed in level!!!!");
		JHNET_LOG(Error, "Only one of LobbyManager must be placed in level!!!!");
	}
	else {
		LobbyManager = (ALobbyManager*)actors[0];
	}

	// Skip Check
	CConfigParser parser(NETWORK_CONFIG_PATH);
	if (parser.IsSuccess()) {
		if (parser.GetBool("singleplay")) {
			StartGame();
		}
	}
}

void ALobbyNetworkProcessor::RecvProc(FReciveData& data)
{
	Super::RecvProc(data);

	int cursor = 0, currentCursor;
	while (cursor < data.len) {
		int bufLen = IntDeserialize(data.buf, &cursor);

		currentCursor = cursor;
		FPacketStruct newPacket;
		newPacket.Deserialize(data.buf, &currentCursor);

		switch (newPacket.type)
		{
		case EMessageType::S_Room_Info:
		{
			Room_Info(data, cursor);
			break;
		}
		case EMessageType::S_Room_ChangeState:
		{
			Room_ChangeState(data, cursor);
			break;
		}
		case EMessageType::S_Lobby_InviteFriend_Request:
		{
			Lobby_InviteFriendRequest(data, cursor);
			break;
		}
		case EMessageType::S_Lobby_InviteFriend_Failed:
		{
			Lobby_InviteFriendFailed(data, cursor);
			break;
		}
		case EMessageType::S_Lobby_MatchAnswer:
		{
			Lobby_MatchAnswer(data, cursor);
			break;
		}
		case EMessageType::S_Lobby_GameStart:
		{
			Lobby_GameStart(data, cursor);
			return;
		}
		case EMessageType::S_Reconnect:
		{
			Reconnect(data, cursor);
			return;
		}
		default:
		{
			cursor += bufLen;
			break;
		}
		}
	}
}

void ALobbyNetworkProcessor::Room_Info(FReciveData& data, int& cursor)
{
	FS_Room_Info sRoomInfo;
	sRoomInfo.Deserialize(data.buf, &cursor);

	// Get 4 UINT64, and insert to slot.
	UINT64 slot;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		slot = sRoomInfo.slot[i];
		LobbyManager->ChangeSlot(i, slot);
	}
	LobbyManager->RefreshLobby();
}

void ALobbyNetworkProcessor::Room_ChangeState(FReciveData& data, int& cursor)
{
	FS_Room_ChangeState sRoomChagneState;
	sRoomChagneState.Deserialize(data.buf, &cursor);

	// Get 1 int and 1 UINT64, and modify the slot.
	int& slot = sRoomChagneState.slot;
	UINT64 steamID = sRoomChagneState.steamID;
	LobbyManager->ChangeSlot(slot, steamID);
	LobbyManager->RefreshLobby();
}

void ALobbyNetworkProcessor::Lobby_InviteFriendRequest(FReciveData& data, int& cursor)
{
	FS_Lobby_InviteFriend_Request sLobbyInviteFriendRequest;
	sLobbyInviteFriendRequest.Deserialize(data.buf, &cursor);

	UINT64& senderID = sLobbyInviteFriendRequest.senderID;
	FString stringSenderID = UINT64ToFString(senderID);
	FString name;
	if (LobbyManager->GetFriendBySteamId(senderID)) name = LobbyManager->GetFriendBySteamId(senderID)->name;
	else name = "Unknown";
	LobbyManager->OpenRequestInviteWG(stringSenderID, name);
}

void ALobbyNetworkProcessor::Lobby_InviteFriendFailed(FReciveData& data, int& cursor)
{
	FS_Lobby_InviteFriend_Failed sLobbyInviteFriendFailed;
	sLobbyInviteFriendFailed.Deserialize(data.buf, &cursor);

	LobbyManager->OpenLobbyWGFailed(sLobbyInviteFriendFailed.msg.buf);
}

void ALobbyNetworkProcessor::Lobby_MatchAnswer(FReciveData& data, int& cursor)
{
	FS_Lobby_MatchAnswer sLobbyMatchAnswer;
	sLobbyMatchAnswer.Deserialize(data.buf, &cursor);

	bool& isOn = sLobbyMatchAnswer.isOn;
	int& slot = sLobbyMatchAnswer.slot;
	LobbyManager->SetReadyButton(slot, isOn);
}

void ALobbyNetworkProcessor::Lobby_GameStart(FReciveData& data, int& cursor)
{
	FS_Lobby_GameStart sLobbyGameStart;
	sLobbyGameStart.Deserialize(data.buf, &cursor);
	
	StartGame();
}

void ALobbyNetworkProcessor::StartGame()
{
	if (!fadeScreen->IsInViewport()) fadeScreen->AddToViewport(fadeZIndex);
	if (fadeScreen) fadeScreen->FadeOut(1.8f);

	LobbyManager->OnStartGame();
	GetWorld()->GetTimerManager().SetTimer(_gameStartTimer, this, &ALobbyNetworkProcessor::OnGameStart, 2.0f, false);

	// 다음 메세지는 InGame에서 처리하기위해서 Recv를 더이상 실행하지 않는다.
	DisableRecv(true);
}

void ALobbyNetworkProcessor::OnGameStart()
{
	UGameplayStatics::OpenLevel(GetWorld(), GameName);
}

void ALobbyNetworkProcessor::Reconnect(FReciveData& data, int& cursor)
{
	FS_Reconnect sReconnect;
	sReconnect.Deserialize(data.buf, &cursor);

	for (int i = 0; i < MAX_PLAYER; ++i) {
		GetNetworkSystem()->SetSteamID(i, sReconnect.slot[i]);
	}

	StartGame();
	GetNetworkSystem()->SetReconnectMode(true);
}