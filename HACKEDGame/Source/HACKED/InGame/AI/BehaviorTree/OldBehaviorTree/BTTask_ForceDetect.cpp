// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "BTTask_ForceDetect.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/AI/HACKED_AI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

EBTNodeResult::Type UBTTask_ForceDetect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* AIHACKED_AI = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == AIHACKED_AI) return EBTNodeResult::Failed;

	AHACKED_AI* HACKED_AI = Cast<AHACKED_AI>(AIHACKED_AI);
	if (nullptr == HACKED_AI)
		return EBTNodeResult::Failed;

	UWorld* World = AIHACKED_AI->GetWorld();
	FVector Center = AIHACKED_AI->GetActorLocation();
	float DetectRadius = HACKED_AI->GetDetectingRange();

	TTuple<AActor*, float> nearestPlayer = HACKED_AI->GetNearestPlayer();
	if (nearestPlayer.Key == nullptr) {
		return EBTNodeResult::Failed;
	}
	// Set Target
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(ASMR_AIController::TargetKey, nearestPlayer.Key);
	// Check target is in attack range.
	if (nearestPlayer.Value <= HACKED_AI->GetAttackRange())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, false);
	}
	return EBTNodeResult::Succeeded;
}