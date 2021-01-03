// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SDAI_Attack.h"
#include "InGame/AI/SMR_AIController.h"
#include "../../SelfDestructAI/SelfDestructAI.h"
#include "../../SelfDestructAI/SD_AI_AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InGame/AI/HACKED_AI.h"

UBTTask_SDAI_Attack::UBTTask_SDAI_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SDAI_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASelfDestructAI* SelfDestructAI = Cast<ASelfDestructAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SelfDestructAI)
		return EBTNodeResult::Failed;

	SelfDestructAI->SelfDestruct();

	return EBTNodeResult::Succeeded;

}

void UBTTask_SDAI_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	ASelfDestructAI* SelfDestructAI = Cast<ASelfDestructAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SelfDestructAI)
		return;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ASMR_AIController::TargetKey));
	if (nullptr == Target)
		return;

	FVector LookVector = Target->GetActorLocation() - SelfDestructAI->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	SelfDestructAI->SetActorRotation(FMath::RInterpTo(SelfDestructAI->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));
}
