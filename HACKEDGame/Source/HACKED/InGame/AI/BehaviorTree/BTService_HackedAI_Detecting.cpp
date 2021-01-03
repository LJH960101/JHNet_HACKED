// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_HackedAI_Detecting.h"

UBTService_HackedAI_Detecting::UBTService_HackedAI_Detecting()
{
	NodeName = TEXT("HackedAI Detecting");
	Interval = 0.2f;
}

void UBTService_HackedAI_Detecting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	HACKED_AI = Cast<AHACKED_AI>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == HACKED_AI) return;

	nearestPlayer = HACKED_AI->GetNearestPlayer();
	if (nearestPlayer.Key == nullptr) {
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASMR_AIController::TargetKey, nullptr);
		return;
	}

	if (HACKED_AI->bPatrolCheck)
	{
		_detectRadius = HACKED_AI->GetDetectingRange();
		if (HACKED_AI->bTakenDamage)
		{
			_detectRadius = 2 * (HACKED_AI->GetDetectingRange());
		}
		UWorld* World = HACKED_AI->GetWorld();
		FVector Center = HACKED_AI->GetActorLocation();

		if (nullptr == World) return;

		TArray<FOverlapResult> OverlapResults;
		FCollisionQueryParams CollisionQueryParam(NAME_None, false, HACKED_AI);
		bool bResult = World->OverlapMultiByChannel(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel17,
			FCollisionShape::MakeSphere(_detectRadius),
			CollisionQueryParam);

		if (bResult)
		{
			for (auto PcOverlapResult : OverlapResults)
			{
				AHACKEDCharacter* HackedCharacter = Cast<AHACKEDCharacter>(PcOverlapResult.GetActor());
				if (HackedCharacter && HackedCharacter->GetController()->IsPlayerController())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASMR_AIController::TargetKey, nearestPlayer.Key);
					HACKED_AI->bPatrolCheck = false;
				}
			}
		}
	}

	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASMR_AIController::TargetKey, nearestPlayer.Key);
	}
}
