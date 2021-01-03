// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_NamedAI_Detecting.h"
#include "InGame/AI/MilitaryNamedAI/MilitaryNamedAI.h"
#include "DrawDebugHelpers.h"

UBTService_NamedAI_Detecting::UBTService_NamedAI_Detecting()
{
	NodeName = TEXT("NamedAI State Service");
	Interval = 0.2f;
}

void UBTService_NamedAI_Detecting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AMilitaryNamedAI* MN_AI = Cast<AMilitaryNamedAI>(HACKED_AI);
	if (nullptr == MN_AI) return;

	if (nearestPlayer.Value > MN_AI->PhotonShotRange || 
		(nearestPlayer.Value <= MN_AI->SwapStateRange && nearestPlayer.Value > MN_AI->RagingBlowRange))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MN_AI_PhotonShot, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MN_AI_RagingBlow, false);
	}
	else if (nearestPlayer.Value <= MN_AI->PhotonShotRange && nearestPlayer.Value > MN_AI->SwapStateRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MN_AI_PhotonShot, true);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MN_AI_RagingBlow, false);
	}
	else if (nearestPlayer.Value <= MN_AI->RagingBlowRange)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MN_AI_PhotonShot, false);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::MN_AI_RagingBlow, true);
	}


}
