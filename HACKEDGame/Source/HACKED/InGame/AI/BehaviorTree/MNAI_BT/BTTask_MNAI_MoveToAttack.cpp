// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MNAI_MoveToAttack.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/MilitaryNamedAI/MilitaryNamedAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

UBTTask_MNAI_MoveToAttack::UBTTask_MNAI_MoveToAttack()
{
	NodeName = TEXT("MilitaryNamedAI Move To Attack");
}

EBTNodeResult::Type UBTTask_MNAI_MoveToAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AHACKED_AI* HACKED_AI = Cast<AHACKED_AI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == HACKED_AI)
		return EBTNodeResult::Failed;

	UNavigationSystemV1 * NavSystem = UNavigationSystemV1::GetNavigationSystem(HACKED_AI->GetWorld());
	if (nullptr == NavSystem)
		return EBTNodeResult::Failed;

	TTuple<AActor*, float> nearestPlayer = HACKED_AI->GetNearestPlayer();
	if (nearestPlayer.Key == nullptr) return EBTNodeResult::Failed;

	FVector OriginPos = HACKED_AI->GetActorLocation();
	FVector NextMovePos;

	NextMovePos = (HACKED_AI->NearestPlayerPos) - (HACKED_AI->GetActorLocation());
	NextMovePos.Normalize();
	NextMovePos *= 500.0f;
	NextMovePos += HACKED_AI->GetActorLocation();

	if ((OwnerComp.GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MN_AI_PhotonShot)))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ASMR_AIController::MoveToAttack, NextMovePos);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
