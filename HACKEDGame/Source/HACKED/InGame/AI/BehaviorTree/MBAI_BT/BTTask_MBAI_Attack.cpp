// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MBAI_Attack.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBoss_AIController.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MBAI_Attack::UBTTask_MBAI_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MBAI_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AMilitaryBossAI* MilitaryBossAI = Cast<AMilitaryBossAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryBossAI)
		return EBTNodeResult::Failed;

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(AMilitaryBoss_AIController::PrimaryAttack))
	{
		MilitaryBossAI->HammerShot(&OwnerComp);
		MilitaryBossAI->OnHammerShotEnd.BindUObject(this, &UBTTask_MBAI_Attack::HammerShotEnd);
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_MBAI_Attack::HammerShotEnd(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !(OwnerComp->GetAIOwner()) || !(OwnerComp->GetAIOwner()->GetPawn()))
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);

	AMilitaryBossAI* MilitaryBossAI = Cast<AMilitaryBossAI>(OwnerComp->GetAIOwner()->GetPawn());
	if (nullptr == MilitaryBossAI)
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_MBAI_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AMilitaryBossAI* MilitaryBossAI = Cast<AMilitaryBossAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryBossAI)
		return;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMilitaryBoss_AIController::TargetPlayer));
	if (nullptr == Target)
		return;

	//  액터를 회전시킴.
	FVector LookVector = Target->GetActorLocation() - MilitaryBossAI->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	MilitaryBossAI->SetActorRotation(FMath::RInterpTo(MilitaryBossAI->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 12.5f));

}
