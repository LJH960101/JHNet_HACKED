// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "Core/HACKEDGameInstance.h"
#include "InGame/HACKEDInGameManager.h"
#include "Core/Network/NetworkSystem.h"
#include "InGame/HACKEDTool.h"

using namespace ActorFinderTool;

UHACKEDGameInstance::UHACKEDGameInstance() {
}
UHACKEDGameInstance::~UHACKEDGameInstance() {}

AHACKEDInGameManager* UHACKEDGameInstance::GetHACKEDInGameManager()
{
	if (!HACKEDInGameManager) {
		HACKEDInGameManager = CreateOrGetObject<AHACKEDInGameManager>(GetWorld());
		if (GetNetworkSystem()) GetNetworkSystem()->SetInGameManager(HACKEDInGameManager);
	}
	return HACKEDInGameManager;
}