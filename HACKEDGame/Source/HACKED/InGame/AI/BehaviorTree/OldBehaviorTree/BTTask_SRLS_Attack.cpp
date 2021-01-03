// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SRLS_Attack.h"
#include "../../SR_LS/SecurityRobot_LS.h"
#include "../../SMR_AIController.h" 
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_SRLS_Attack::UBTTask_SRLS_Attack()
{
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_SRLS_Attack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto SecurityRobot_LS = Cast<ASecurityRobot_LS>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SecurityRobot_LS)
		return EBTNodeResult::Failed;

	SecurityRobot_LS->LS_Attack(&OwnerComp);
	SecurityRobot_LS->OnAttackEnd.BindUObject(this, &UBTTask_SRLS_Attack::AttackEnd);
	return EBTNodeResult::InProgress;
}

void UBTTask_SRLS_Attack::AttackEnd(UBehaviorTreeComponent* OwnerComp) {
	auto SecurityRobot_LS = Cast<ASecurityRobot_LS>(OwnerComp->GetAIOwner()->GetPawn());
	if (nullptr == SecurityRobot_LS)
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);

	TTuple<AActor*, float> playerTuple = SecurityRobot_LS->GetNearestPlayer();
	if (playerTuple.Key == nullptr) {
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		return;
	}

	LOG_SCREEN("Check");
	// Refresh Attack allow.
	if (playerTuple.Value < SecurityRobot_LS->GetAttackRange())
		OwnerComp->GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, true);
	else
		OwnerComp->GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, false);

	// If can attack?
	if (OwnerComp->GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::AttackAllow)) {
		// Do next attack.
		SecurityRobot_LS->LS_NextAttack();
		SecurityRobot_LS->OnAttackEnd.BindUObject(this, &UBTTask_SRLS_Attack::AttackEnd);
	}
	else FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}