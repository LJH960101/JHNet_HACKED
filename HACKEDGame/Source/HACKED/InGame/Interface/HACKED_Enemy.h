// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HACKED_Enemy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHACKED_Enemy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HACKED_API IHACKED_Enemy
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	virtual void OnEnableAI(bool isEnable) {}
};
