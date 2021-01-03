// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_AI_Patrol.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/HACKED_AI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_AI_Patrol::UBTTask_AI_Patrol()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTTask_AI_Patrol::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	APawn* AI = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == AI)
		return EBTNodeResult::Failed;

	AHACKED_AI* HACKED_AI = Cast<AHACKED_AI>(AI);
	if (nullptr == HACKED_AI)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(AI->GetWorld());
	if (nullptr == NavSystem)
		return EBTNodeResult::Failed;

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ASMR_AIController::HomePosKey);
	FNavLocation NextPatrol;

	// 자신의 현위치를 고수한 상태로의 Patrol을 진행한다.
	if (NavSystem->GetRandomPointInNavigableRadius(/*FVector::ZeroVector*/Origin, HACKED_AI->GetAIPatrolRange(), NextPatrol))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ASMR_AIController::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
