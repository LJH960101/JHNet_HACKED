// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MSAI_Attack.h"
#include "InGame/AI/SMR_AIController.h"
#include "../../MilitarySoldierAI/MilitarySoldierAI.h"
#include "../../MilitarySoldierAI/MS_AI_AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InGame/AI/HACKED_AI.h"

UBTTask_MSAI_Attack::UBTTask_MSAI_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MSAI_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AMilitarySoldierAI* MilitarySoliderAI = Cast<AMilitarySoldierAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitarySoliderAI)
		return EBTNodeResult::Failed;

	// 원거리
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MS_AI_LongAttack))
	{
		//LOG_SCREEN("Laser Attack Result");
		MilitarySoliderAI->LaserShock();
	}
	// 근거리
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MS_AI_ShortAttack))
	{
		//LOG_SCREEN("Blade Attack Result");
		MilitarySoliderAI->BladeShock(&OwnerComp);
		MilitarySoliderAI->OnBladeShockEnded.BindUObject(this, &UBTTask_MSAI_Attack::BladeAttackEnd);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MSAI_Attack::BladeAttackEnd(UBehaviorTreeComponent* OwnerComp)
{
	if(!OwnerComp || !(OwnerComp->GetAIOwner()) || !(OwnerComp->GetAIOwner()->GetPawn())) FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	AMilitarySoldierAI* MilitaryRobot_AI = Cast<AMilitarySoldierAI>(OwnerComp->GetAIOwner()->GetPawn());
	if (nullptr == MilitaryRobot_AI)
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);

	if (OwnerComp->GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::MS_AI_ShortAttack))
	{
		MilitaryRobot_AI->BladeNextAttack();
		MilitaryRobot_AI->OnBladeShockEnded.BindUObject(this, &UBTTask_MSAI_Attack::BladeAttackEnd);
	}
	else FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);

}

void UBTTask_MSAI_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	auto MilitarySoliderAI = Cast<AMilitarySoldierAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitarySoliderAI)
		return;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ASMR_AIController::TargetKey));
	if (nullptr == Target)
		return;

	//  액터를 회전시킴.
	FVector LookVector = Target->GetActorLocation() - MilitarySoliderAI->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	MilitarySoliderAI->SetActorRotation(FMath::RInterpTo(MilitarySoliderAI->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 12.5f));

	// 레이저가 끝났는지 체크한다.
	if (MilitarySoliderAI->bOnEndLaserShock) {
		LOG_SCREEN("CHECK");
		MilitarySoliderAI->bOnEndLaserShock = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
