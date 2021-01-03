// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MBAI_PatternAttack.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBoss_AIController.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MBAI_PatternAttack::UBTTask_MBAI_PatternAttack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MBAI_PatternAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AMilitaryBossAI* MilitaryBossAI = Cast<AMilitaryBossAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryBossAI)
		return EBTNodeResult::Failed;

	LOG_SCREEN("MilitaryBossAI ExecuteTask Check");
	int patternChoose = FMath::RandRange(0, 3);

	LOG_SCREEN("Pattern Check Num : %d", patternChoose);
	switch (patternChoose)
	{
	case 0 :
		MilitaryBossAI->MachineGun();
		break;
	case 1:
		MilitaryBossAI->HomingMissile();
		break;
	case 2:
		MilitaryBossAI->SlapRush();
		MilitaryBossAI->GetCharacterMovement()->MaxWalkSpeed = (MilitaryBossAI->_moveSpeed) * 2;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::SlapRush, true);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		break;
	default:
		MilitaryBossAI->MachineGun();
		break;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MBAI_PatternAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AMilitaryBossAI* MilitaryBossAI = Cast<AMilitaryBossAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == MilitaryBossAI)
		return;

	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AMilitaryBoss_AIController::TargetPlayer));
	if (nullptr == Target)
		return;

	FVector LookVector = Target->GetActorLocation() - MilitaryBossAI->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	MilitaryBossAI->SetActorRotation(FMath::RInterpTo(MilitaryBossAI->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 5.0f));


	if (MilitaryBossAI->bOnEndMachineGun)
	{
		MilitaryBossAI->bOnEndMachineGun = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (MilitaryBossAI->bOnEndHomingMissile)
	{
		MilitaryBossAI->bOnEndHomingMissile = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (MilitaryBossAI->bOnEndSlapRush)
	{
		MilitaryBossAI->bOnEndSlapRush = false;
		MilitaryBossAI->GetCharacterMovement()->MaxWalkSpeed = MilitaryBossAI->_moveSpeed;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::SlapRush, false);
		//FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
