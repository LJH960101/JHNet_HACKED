// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 


#include "BTTask_SRSLS_Attack.h"
#include "InGame/AI/SMR_AIController.h"
#include "../../SR_SLS/SecurityRobot_SLS.h"

UBTTask_SRSLS_Attack::UBTTask_SRSLS_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SRSLS_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto SecurityRobotSLS = Cast<ASecurityRobot_SLS>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SecurityRobotSLS)
		return EBTNodeResult::Failed;

	SecurityRobotSLS->SLSAttack();
	IsAttacking = true;
	SecurityRobotSLS->OnSLSAttackEnd.AddLambda([this]()->void 
	{
		IsAttacking = false;
	});

	return EBTNodeResult::InProgress;
}

void UBTTask_SRSLS_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
