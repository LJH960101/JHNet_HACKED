// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MNAI_Attack.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/MilitaryNamedAI/MilitaryNamedAI.h"
#include "InGame/AI/MilitaryNamedAI/MN_AI_AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InGame/AI/HACKED_AI.h"

UBTTask_MNAI_Attack::UBTTask_MNAI_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MNAI_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AMilitaryNamedAI* MilitaryNamedAI = Cast<AMilitaryNamedAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryNamedAI)
		return EBTNodeResult::Failed;

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MN_AI_PhotonShot))
	{
		MilitaryNamedAI->PhotonShot();
	}

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MN_AI_RagingBlow))
	{
		MilitaryNamedAI->RagingBlow(&OwnerComp);
		MilitaryNamedAI->OnRagingBlowEnded.BindUObject(this, &UBTTask_MNAI_Attack::RagingBlowEnd);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MNAI_Attack::RagingBlowEnd(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !(OwnerComp->GetAIOwner()) || !(OwnerComp->GetAIOwner()->GetPawn())) 
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	AMilitaryNamedAI * MilitaryNamedAI = Cast<AMilitaryNamedAI>(OwnerComp->GetAIOwner()->GetPawn());
	if (nullptr == MilitaryNamedAI)
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);

	if (OwnerComp->GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MN_AI_RagingBlow))
	{
		MilitaryNamedAI->RagingBlowNextRide();
		MilitaryNamedAI->OnRagingBlowEnded.BindUObject(this, &UBTTask_MNAI_Attack::RagingBlowEnd);
	}
	else FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_MNAI_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AMilitaryNamedAI* MilitaryNamedAI = Cast<AMilitaryNamedAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryNamedAI)
		return;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ASMR_AIController::TargetKey));
	if (nullptr == Target)
		return;

	//  액터를 회전시킴.
	FVector LookVector = Target->GetActorLocation() - MilitaryNamedAI->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	MilitaryNamedAI->SetActorRotation(FMath::RInterpTo(MilitaryNamedAI->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 12.5f));

	// 레이저가 끝났는지 체크한다.
	if (MilitaryNamedAI->bOnEndPhotonShot) 
	{
		//LOG_SCREEN("CHECK");
		MilitaryNamedAI->bOnEndPhotonShot = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
