// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SoldierAI_Detecting.h"
#include "InGame/AI/MilitarySoldierAI/MilitarySoldierAI.h"
#include "DrawDebugHelpers.h"

UBTService_SoldierAI_Detecting::UBTService_SoldierAI_Detecting()
{
	NodeName = TEXT("SoldierAI State Service");
	Interval = 0.2f;
}

void UBTService_SoldierAI_Detecting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AMilitarySoldierAI* MS_AI = Cast<AMilitarySoldierAI>(HACKED_AI);
	if (nullptr == MS_AI) return;


	if (nearestPlayer.Value > MS_AI->LaserAttackRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_LongAttack, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_ShortAttack, false);
	}

	else if (nearestPlayer.Value <= MS_AI->LaserAttackRange && nearestPlayer.Value > MS_AI->SwapAttackRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_LongAttack, true);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_ShortAttack, false);
		MS_AI->GetCharacterMovement()->MaxWalkSpeed = MS_AI->GetAIMoveSpeed();
	}
	else if (nearestPlayer.Value <= MS_AI->SwapAttackRange && nearestPlayer.Value > MS_AI->BladeAttackRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_LongAttack, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_ShortAttack, false);
		MS_AI->GetCharacterMovement()->MaxWalkSpeed = MS_AI->GetAIMoveSpeed() + 300.0f;
	}
	else if (nearestPlayer.Value <= MS_AI->BladeAttackRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_ShortAttack, true);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MS_AI_LongAttack, false);
	}

}


