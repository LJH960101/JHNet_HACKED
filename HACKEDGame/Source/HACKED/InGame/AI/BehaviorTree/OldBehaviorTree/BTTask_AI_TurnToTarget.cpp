// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_AI_TurnToTarget.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "InGame/AI/SR_LS/SecurityRobot_LS.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AI_TurnToTarget::UBTTask_AI_TurnToTarget()
{
	NodeName = TEXT("AI_Turn");
}

EBTNodeResult::Type UBTTask_AI_TurnToTarget::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AHACKED_AI* HACKED_AI = Cast<AHACKED_AI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == HACKED_AI)
		return EBTNodeResult::Failed;

	AHACKEDCharacter* Target = Cast<AHACKEDCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ASMR_AIController::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - HACKED_AI->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
		
	HACKED_AI->SetActorRotation(FMath::RInterpTo(HACKED_AI->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;

	return EBTNodeResult::Type();
}
