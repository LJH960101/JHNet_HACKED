// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SRET_Attack.h"
#include "InGame/AI/SMR_AIController.h"
#include "../../SR_ET/SecurityRobot_ET.h"
#include "../../SR_ET/SR_ET_AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InGame/AI/HACKED_AI.h"


UBTTask_SRET_Attack::UBTTask_SRET_Attack()
{
	bNotifyTick = true;
}



EBTNodeResult::Type UBTTask_SRET_Attack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto SecurityRobot_ET = Cast<ASecurityRobot_ET>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SecurityRobot_ET)
		return EBTNodeResult::Failed;

	SecurityRobot_ET->ElectricShock();

	return EBTNodeResult::InProgress;
}

void UBTTask_SRET_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	auto securityET = Cast<ASecurityRobot_ET>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == securityET)
		return ;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ASMR_AIController::TargetKey));
	if (nullptr == Target)
		return ;

	FVector LookVector = Target->GetActorLocation() - securityET->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	securityET->SetActorRotation(FMath::RInterpTo(securityET->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	if (securityET->bOnEndAttack) {
		securityET->bOnEndAttack = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}