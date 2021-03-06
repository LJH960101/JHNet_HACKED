// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 


#include "BTDecorator_AI_AllStop.h"
#include "InGame/AI/SMR_AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTDecorator_AI_AllStop::UBTDecorator_AI_AllStop()
{
	NodeName = TEXT("AI All Stop");
}

EBlackboardNotificationResult UBTDecorator_AI_AllStop::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = (UBehaviorTreeComponent*)Blackboard.GetBrainComponent();
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (BlackboardKey.GetSelectedKeyID() == ChangedKeyID)
	{
		BehaviorComp->RequestExecution(this);
	}
	return EBlackboardNotificationResult::ContinueObserving;


	//EBlackboardNotificationResult Result = Super::OnBlackboardKeyValueChange(Blackboard, ChangedKeyID);
	//OwnerComp.GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AttackAllow, true);
	//Blackboard.SetValueAsBool(ASMR_AIController::AllStop, true);
	//UBlackboardComponent& AllStopCheck = const_cast<UBlackboardComponent&>(Blackboard);
	//AllStopCheck.SetValueAsBool(UBTDecorator_AI_AllStop::AllStop, false);

	//return EBlackboardNotificationResult();
}
