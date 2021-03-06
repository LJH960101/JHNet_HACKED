// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Core/JHNETGameInstance.h"
#include "InGame/InGameManager.h"
#include "Network/NetworkSystem.h"
#include "InGame/JHNETTool.h"

using namespace ActorFinderTool;

UJHNETGameInstance::UJHNETGameInstance() {
	_networkSystem = CreateDefaultSubobject<UNetworkSystem>(TEXT("NetworkSystem"));
}
UJHNETGameInstance::~UJHNETGameInstance() {}

UNetworkSystem* UJHNETGameInstance::GetNetworkSystem(){
	return _networkSystem;
}

AInGameManager* UJHNETGameInstance::GetInGameManager()
{
	if (!InGameManager) {
		InGameManager = CreateOrGetObject<AInGameManager>(GetWorld());
		if (_networkSystem) _networkSystem->SetInGameManager(InGameManager);
	}
	return InGameManager;
}

void UJHNETGameInstance::Init() {
	Super::Init();
	if (_networkSystem) _networkSystem->Init();
	else JHNET_LOG(Error, "Can't create NetworkSystem");
}

void UJHNETGameInstance::Shutdown()
{
	Super::Shutdown();
	if (_networkSystem) _networkSystem->Shutdown();
}