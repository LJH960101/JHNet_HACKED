// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AI_MoveToAttack.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/MilitarySoldierAI/MilitarySoldierAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"

UBTTask_AI_MoveToAttack::UBTTask_AI_MoveToAttack()
{
	NodeName = TEXT("MilitarySoldierAI Move To Attack");
}

EBTNodeResult::Type UBTTask_AI_MoveToAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AHACKED_AI* HACKED_AI = Cast<AHACKED_AI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == HACKED_AI)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(HACKED_AI->GetWorld());
	if (nullptr == NavSystem)
		return EBTNodeResult::Failed;

	TTuple<AActor*, float> nearestPlayer = HACKED_AI->GetNearestPlayer();
	if (nearestPlayer.Key == nullptr) return EBTNodeResult::Failed;

	FVector OriginPos = HACKED_AI->GetActorLocation();
	FVector NextMovePos;

	NextMovePos = (HACKED_AI->NearestPlayerPos) - (HACKED_AI->GetActorLocation());
	NextMovePos.Normalize();
	NextMovePos *= 300.0f;
	NextMovePos += HACKED_AI->GetActorLocation();

	if ((OwnerComp.GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MS_AI_LongAttack)))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ASMR_AIController::MoveToAttack, NextMovePos);
		return EBTNodeResult::Succeeded;
	}


	return EBTNodeResult::Failed;
}
