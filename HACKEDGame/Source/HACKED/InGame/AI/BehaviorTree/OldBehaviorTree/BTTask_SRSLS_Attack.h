// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SRSLS_Attack.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTTask_SRSLS_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SRSLS_Attack();
	

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttacking = false;
};
