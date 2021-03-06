// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 


#include "HACKEDTool.h"
#include "AIController.h"
#include "InGame/AI/SMR_AIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "InGame/Interface/HACKED_Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Core/JHNETGameInstance.h"
#include "Core/Network/NetworkSystem.h"

using namespace AIAllStopTool;

void AIAllStopTool::AIDisable(APawn* ai, bool bWithoutAuthority)
{
	// 네트워크 Authority가 없다면 Disable 하지 않는다.
	if(!bWithoutAuthority){
		UJHNETGameInstance* hackedInstance = Cast<UJHNETGameInstance>(UGameplayStatics::GetGameInstance(ai->GetWorld()));
		UNetworkSystem* networkSystem = nullptr;
		if (hackedInstance) networkSystem = hackedInstance->GetNetworkSystem();
		if (networkSystem) {
			if (networkSystem->GetCurrentSlot() != 0) return;
		}
	}

	AController* hackedController = ai->GetController();
	if (hackedController)
	{
		AAIController* hackedAIController = Cast<AAIController>(hackedController);
		if (hackedAIController != nullptr && hackedAIController->GetBrainComponent() != nullptr)
		{
			hackedAIController->GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AllStop, true);
			hackedAIController->StopMovement();
			hackedAIController->GetBrainComponent()->StopLogic("AI All Stop");

			IHACKED_Enemy* enemy = Cast<IHACKED_Enemy>(ai);
			if (!bWithoutAuthority && enemy) {
				enemy->OnEnableAI(false);
			}
		}
	}
}

void AIAllStopTool::AIRestart(APawn* ai, bool bWithoutAuthority)
{
	
	// 네트워크 Authority가 없다면 Enable 하지 않는다.
	if (!bWithoutAuthority) {
		UJHNETGameInstance* hackedInstance = Cast<UJHNETGameInstance>(UGameplayStatics::GetGameInstance(ai->GetWorld()));
		UNetworkSystem* networkSystem = nullptr;
		if (hackedInstance) networkSystem = hackedInstance->GetNetworkSystem();
		if (networkSystem) {
			if (networkSystem->GetCurrentSlot() != 0) return;
		}
	}

	AController* hackedController = ai->GetController();
	if (hackedController)
	{
		AAIController* hackedAIController = Cast<AAIController>(hackedController);
		if (hackedAIController != nullptr && hackedAIController->GetBrainComponent() != nullptr)
		{
			hackedAIController->MoveToActor(nullptr);
			hackedAIController->GetBrainComponent()->RestartLogic();
			hackedAIController->GetBrainComponent()->GetBlackboardComponent()->SetValueAsBool(ASMR_AIController::AllStop, false);
			
			IHACKED_Enemy* enemy = Cast<IHACKED_Enemy>(ai);
			if (!bWithoutAuthority && enemy) {
				enemy->OnEnableAI(true);
			}
		} 
	}
}

bool AIAllStopTool::OnAIPlaying(APawn* ai)
{
	AController* hackedController = ai->GetController();
	if (hackedController)
	{
		AAIController* hackedAIController = Cast<AAIController>(hackedController);
		if (hackedAIController != nullptr && hackedAIController->GetBrainComponent() != nullptr)
		{
			return hackedAIController->GetBrainComponent()->GetBlackboardComponent()->GetValueAsBool(ASMR_AIController::AllStop);
		}
	}
	return false;
}
