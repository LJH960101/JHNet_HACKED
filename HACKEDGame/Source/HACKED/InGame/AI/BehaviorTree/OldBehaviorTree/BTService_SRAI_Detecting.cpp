// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_SRAI_Detecting.h"


UBTService_SRAI_Detecting::UBTService_SRAI_Detecting()
{
	NodeName = TEXT("SR State Service");
	Interval = 0.2f;
}

void UBTService_SRAI_Detecting::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check target is in attack range.
	if (nearestPlayer.Value <= HACKED_AI->GetAttackRange())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, false);
	}
}
