// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_AI_AllStop.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTDecorator_AI_AllStop : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

public:
	UBTDecorator_AI_AllStop();


	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;
	
};
