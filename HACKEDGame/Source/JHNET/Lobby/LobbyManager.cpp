// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyManager.h"
#include "Widget/WGFriendInfo.h"
#include "Steamworks/Steamv139/sdk/public/steam/steam_api.h"
#include "NetworkModule/Serializer.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"
#include "Common/Widget/WGFailed.h"
#include "Widget/WGFriendRequest.h"
#include "Network/LobbyNetworkProcessor.h"
#include "NetworkModule/NetworkTool.h"
#include <memory>

using std::shared_ptr;
using namespace NetworkTool;
using namespace JHNETSerializer;

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Lobby WG
	static ConstructorHelpers::FClassFinder<UWGLobby> WGLobby(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_LobbyMain.WB_LobbyMain_C'"));
	if (WGLobby.Succeeded()) {
		WG_Lobby_Class = WGLobby.Class;
	}
	else
	{
		JHNET_LOG_S(Error);
	}

	// Friend list WG
	static ConstructorHelpers::FClassFinder<UUserWidget> friendWG(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_LobbyFriendPopUp.WB_LobbyFriendPopUp_C'"));
	if (friendWG.Succeeded()) {
		WG_Friend_Class = friendWG.Class;
	}
	else
	{
		JHNET_LOG_S(Error);
	}

	// Setting WG
	static ConstructorHelpers::FClassFinder<UUserWidget> settingWG(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_LobbySetting.WB_LobbySetting_C'"));
	if (settingWG.Succeeded()) {
		WG_Setting_Class = settingWG.Class;
	}
	else
	{
		JHNET_LOG_S(Error);
	}

	// Invite Request WG
	static ConstructorHelpers::FClassFinder<UWGFriendRequest> inviteRequestWG(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_LobbyFriendRequest.WB_LobbyFriendRequest_C'"));
	if (inviteRequestWG.Succeeded()) {
		WG_RequestInvite_Class = inviteRequestWG.Class;
	}
	else
	{
		JHNET_LOG_S(Error);
	}

	// Failed WG
	static ConstructorHelpers::FClassFinder<UWGFailed> WGFailed(TEXT("WidgetBlueprint'/Game/NetworkUI/WB_Failed.WB_Failed_C'"));
	if (WGFailed.Succeeded()) {
		WG_Failed_Class = WGFailed.Class;
	}
	else
	{
		JHNET_LOG_S(Error);
	}
}
void ALobbyManager::RefreshList()
{
	friendInfos->RefreshFriendInfo();
}

// Called when the game starts or when spawned
void ALobbyManager::BeginPlay()
{
	Super::BeginPlay();

	friendInfos = NewObject<UWGFriendInfo>(this);
	friendInfos->RefreshFriendInfo();

	if (WG_Lobby == nullptr) {
		WG_Lobby = CreateWidget<UWGLobby>(GetWorld(), WG_Lobby_Class);
		WG_Lobby->AddToViewport();
	}

	// Create and init slots
	for (int i = 0; i < MAX_PLAYER; ++i) {
		URoomPlayer* slot = NewObject<URoomPlayer>(URoomPlayer::StaticClass());
		slots[i] = slot;
		ChangeSlot(i, gameInstance->GetNetworkSystem()->GetSteamID(i));
	}
	RefreshLobby();

	// Error when NetworkProcessor not spawned.
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyNetworkProcessor::StaticClass(), outActors);
	if (outActors.Num() <= 0) {
		JHNET_LOG(Error, "Can't get LobbyNetworkProcessor.");
	}
	else {
		_lobbyNetworkProcessor = Cast<ALobbyNetworkProcessor>(outActors[0]);
	}
}

void ALobbyManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	gameInstance = Cast<UJHNETGameInstance>(GetGameInstance());
}

int ALobbyManager::GetFriendLen()
{
	return friendInfos->FriendInformations.Num();
}

FString ALobbyManager::GetFriendName(const int& number)
{
	if (GetFriendLen() <= number) return "0";
	return friendInfos->FriendInformations[number]->name;
}

FString ALobbyManager::GetFriendSteamID(const int& number)
{
	return  UINT64ToFString(friendInfos->FriendInformations[number]->steamID);
}

UTexture2D* ALobbyManager::GetFriendImage(const int& number)
{
	if (GetFriendLen() <= number) return nullptr;
	return friendInfos->FriendInformations[number]->avatar;
}

void ALobbyManager::InviteFriend(FString steamID)
{
	FC_Lobby_InviteFriend_Request cLobbyInviteFriendRequest;
	cLobbyInviteFriendRequest.targetID = FStringToUINT64(steamID);
	if(_lobbyNetworkProcessor) _lobbyNetworkProcessor->Send((FPacketStruct*)&cLobbyInviteFriendRequest);
}

void ALobbyManager::AnswerInviteRequest(bool isYes, const FString& steamID)
{
	FC_Lobby_InviteFriend_Answer cLobbyInviteFriendAnswer;
	cLobbyInviteFriendAnswer.isAccept = isYes;
	cLobbyInviteFriendAnswer.targetID = FStringToUINT64(steamID);
	if (_lobbyNetworkProcessor) _lobbyNetworkProcessor->Send((FPacketStruct*)&cLobbyInviteFriendAnswer);
}

UFriendInformation* ALobbyManager::GetFriendBySteamId(const UINT64& steamID)
{
	return friendInfos->GetFriendInformationBySteamID(steamID);
}

void ALobbyManager::ChangeSlot(int slotNumber, const UINT64 & steamID)
{
	// Empty the exited slot
	if (steamID == 0) {
		slots[slotNumber]->name = "";
		slots[slotNumber]->steamID = "0";
		slots[slotNumber]->image = nullptr;
		slots[slotNumber]->onReady = false;
	}
	else {
		slots[slotNumber]->steamID = UINT64ToFString(steamID);
		slots[slotNumber]->name = gameInstance->GetNetworkSystem()->GetSteamName(steamID);
		slots[slotNumber]->image = gameInstance->GetNetworkSystem()->GetSteamAvartar(steamID);
		slots[slotNumber]->onReady = false;
	}
}

void ALobbyManager::RefreshLobby()
{
	bool bAllReady = true;
	// Refresh all slots
	for (int i = 0; i < MAX_PLAYER; ++i) {
		WG_Lobby->SetPartySlot(i, slots[i]->name, slots[i]->image, slots[i]->steamID, slots[i]->onReady);
		if (slots[i]->onReady == false && slots[i]->steamID != "0") bAllReady = false;
	}
	if (slots[0]->steamID == "0" || !gameInstance->GetNetworkSystem()->OnServer()) bAllReady = false;
	WG_Lobby->OnMatching(bAllReady);
	OnMatching(bAllReady);
}

void ALobbyManager::SetReadyButton(const int& slot, const bool& isOn)
{
	if (slot >= MAX_PLAYER || slot < 0) return;

	// if my slot, Change the ready button
	if (FStringToUINT64(slots[slot]->steamID) == gameInstance->GetNetworkSystem()->GetSteamID())
		WG_Lobby->OnReady(isOn);
	slots[slot]->onReady = isOn;
	RefreshLobby();
}

void ALobbyManager::OnClickSetting()
{
	if (WG_Setting == nullptr) {
		WG_Setting = CreateWidget<UUserWidget>(GetWorld(), WG_Setting_Class);
		onSetting = false;
	}

	if (onSetting) {
		WG_Setting->RemoveFromParent();
		onSetting = false;
	}
	else {
		WG_Setting->AddToViewport();
		onSetting = true;
	}
}

void ALobbyManager::OpenRequestInviteWG(const FString& steamID, const FString& userName)
{
	if (WG_RequestInvite == nullptr) WG_RequestInvite = CreateWidget<UWGFriendRequest>(GetWorld(), WG_RequestInvite_Class);
	WG_RequestInvite->AddToViewport();
	WG_RequestInvite->SetSteamID(steamID);
	WG_RequestInvite->SetUserName(userName);
}

void ALobbyManager::OpenLobbyWGFailed(const FString& msg)
{
	if (WG_Failed == nullptr) WG_Failed = CreateWidget<UWGFailed>(GetWorld(), WG_Failed_Class);
	WG_Failed->AddToViewport();
}

int32 ALobbyManager::GetMaxPlayer()
{
	return MAX_PLAYER;
}

void ALobbyManager::SetSteamID_DEBUG(const FString & steamID)
{
	gameInstance->GetNetworkSystem()->SetSteamID_DUBGE(FStringToUINT64(steamID));
}

void ALobbyManager::EnterToRoom_DEBUG()
{
	FC_Debug_RoomStart cDebugRoomStart;
	if(_lobbyNetworkProcessor) _lobbyNetworkProcessor->Send((FPacketStruct*)&cDebugRoomStart);
}

void ALobbyManager::ChangePartyKing(const int32& slot, const FString& steamID)
{
	int slotNumb = static_cast<int>(slot);
	UINT64 myId = gameInstance->GetNetworkSystem()->GetSteamID();
	UINT64 uintSteamID = FStringToUINT64(steamID);
	// Only party king can do this proc
	if (slots[0]->steamID != "0" && myId != FStringToUINT64(slots[0]->steamID)) {
		OpenLobbyWGFailed("Only party king can change.");
	}
	// Can't be the target myself.
	if (myId == uintSteamID) {
		OpenLobbyWGFailed("You already party king.");
	}

	FC_Lobby_SetPartyKing cLobbySetPartyKing;
	cLobbySetPartyKing.slot = slotNumb;

	if(_lobbyNetworkProcessor) _lobbyNetworkProcessor->Send((FPacketStruct*)&cLobbySetPartyKing);
}

void ALobbyManager::OnClickFriend()
{
	if (WG_Friend == nullptr) {
		WG_Friend = CreateWidget<UUserWidget>(GetWorld(), WG_Friend_Class);
		onFriend = false;
	}

	if (onFriend) {
		WG_Friend->RemoveFromParent();
		onFriend = false;
	}
	else {
		WG_Friend->AddToViewport();
		onFriend = true;
	}
}

void ALobbyManager::OnReady(bool isOn)
{
	if (slots[0] == nullptr) {
		OpenLobbyWGFailed("Can't connect to server.");
		return;
	}

	FC_Lobby_MatchRequest cLobbyMatchRequest;
	cLobbyMatchRequest.isOn = isOn;

	if (_lobbyNetworkProcessor) _lobbyNetworkProcessor->Send((FPacketStruct*)&cLobbyMatchRequest);
}

int32 ALobbyManager::GetMySlotNumber() 
{
	FString myId = UINT64ToFString(gameInstance->GetNetworkSystem()->GetSteamID());
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (slots[i]->steamID == myId) {
			return i;
		}
	}
	return -1;
}

void ALobbyManager::Kick(const int32& slot, const FString& steamID)
{
	int targetSlot = static_cast<int>(slot);
	UINT64 myId = gameInstance->GetNetworkSystem()->GetSteamID();
	UINT64 uintSteamID = FStringToUINT64(steamID);
	// Only work on kick myself or party king
	if (!(myId == uintSteamID || (slots[0]->steamID != "0" && myId == FStringToUINT64(slots[0]->steamID)))) {
		OpenLobbyWGFailed("Only party king can kick other members.");
		return;
	}

	FC_Lobby_FriendKick_Request cLobbyFriendKickRequest;
	cLobbyFriendKickRequest.slot = targetSlot;

	if (_lobbyNetworkProcessor) _lobbyNetworkProcessor->Send((FPacketStruct*)&cLobbyFriendKickRequest);
}