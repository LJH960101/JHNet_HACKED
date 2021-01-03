// Network receive process class in Lobby.

#pragma once

#include "JHNET.h"
#include "GameFramework/Actor.h"
#include "Common/Network/CommonNetworkProcessor.h"
#include "LobbyNetworkProcessor.generated.h"

class ALobbyManager;

UCLASS()
class JHNET_API ALobbyNetworkProcessor : public ACommonNetworkProcessor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyNetworkProcessor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void RecvProc(FReciveData& data) override;

private:

	ALobbyManager* LobbyManager;

public:	
	UPROPERTY(EditAnywhere, Category ="Lobby")
	FName GameName;

private:
	// �޼��� ó�� �Լ�

	// Room
	void Room_Info(FReciveData& data, int& cursor);
	void Room_ChangeState(FReciveData& data, int& cursor);

	// Lobby
	void Lobby_InviteFriendRequest(FReciveData& data, int& cursor);
	void Lobby_InviteFriendFailed(FReciveData& data, int& cursor);
	void Lobby_MatchAnswer(FReciveData& data, int& cursor);
	void Lobby_GameStart(FReciveData& data, int& cursor);

	FTimerHandle _gameStartTimer;
	void StartGame();
	void OnGameStart();

	void Reconnect(FReciveData& data, int& cursor);
};
