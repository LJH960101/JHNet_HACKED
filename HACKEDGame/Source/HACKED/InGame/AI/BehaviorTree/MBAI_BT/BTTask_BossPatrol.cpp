// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_BossPatrol.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBoss_AIController.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_BossPatrol::UBTTask_BossPatrol()
{
	NodeName = TEXT("Boss Patrol");
}

EBTNodeResult::Type UBTTask_BossPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AMilitaryBossAI* MilitaryBossAI = Cast<AMilitaryBossAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryBossAI)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(MilitaryBossAI->GetWorld());
	if (nullptr == NavSystem)
		return EBTNodeResult::Failed;

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AMilitaryBoss_AIController::HomePos);
	FNavLocation NextPatrol;

	// 자신의 현위치를 고수한 상태로의 Patrol을 진행한다.
	if (NavSystem->GetRandomPointInNavigableRadius(/*FVector::ZeroVector*/Origin, 500.0f, NextPatrol))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AMilitaryBoss_AIController::PatrolPos, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

