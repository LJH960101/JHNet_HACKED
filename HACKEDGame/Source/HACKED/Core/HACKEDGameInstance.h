// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "JHNET.h"
#include "Engine/GameInstance.h"
#include "Core/JHNETGameInstance.h"
#include "HACKEDGameInstance.generated.h"

class UNetworkSystem;
class AHACKEDInGameManager;

UCLASS()
class HACKED_API UHACKEDGameInstance : public UJHNETGameInstance
{
	GENERATED_BODY()
public:
	UHACKEDGameInstance();
	~UHACKEDGameInstance();

	AHACKEDInGameManager* GetHACKEDInGameManager();

private:
	UPROPERTY()
	AHACKEDInGameManager* HACKEDInGameManager;
};
