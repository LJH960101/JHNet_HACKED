// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_BossAI_Detecting.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBoss_AIController.h"

UBTService_BossAI_Detecting::UBTService_BossAI_Detecting()
{
	NodeName = TEXT("MilitaryBoss Agro Detecting");
	Interval = 0.2f;
}

void UBTService_BossAI_Detecting::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AHACKED_AI* HACKED_AI = Cast<AHACKED_AI>(OwnerComp.GetAIOwner()->GetPawn());

	MilitaryBossAI = Cast<AMilitaryBossAI>(HACKED_AI);
	if (nullptr == MilitaryBossAI) return;

	FVector MilitaryBossPos = MilitaryBossAI->GetActorLocation();

	nearestPlayer = MilitaryBossAI->GetNearestPlayer();
	targetPlayer = MilitaryBossAI->GetAgroPlayer();


	if (targetPlayer == nullptr || nearestPlayer.Key == nullptr)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMilitaryBoss_AIController::TargetPlayer, nullptr);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMilitaryBoss_AIController::TargetPlayer, targetPlayer);
		float DistanceTarget = FVector::Dist(MilitaryBossPos, targetPlayer->GetActorLocation());

		//LOG_SCREEN("Distance Target : %f", DistanceTarget);
		//LOG_SCREEN("Check");
		if (MilitaryBossAI->changeToNearTarget)
		{
			LOG_SCREEN("Agro Change to Crusher");
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AMilitaryBoss_AIController::TargetPlayer, nearestPlayer.Key);

			if (nearestPlayer.Value <= MilitaryBossAI->GetHammerShotRange())
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PrimaryAttack, true);
			else
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PrimaryAttack, false);
				MilitaryBossAI->changeToNearTarget = false;
				return;
			}
			return;
		}
		else
		{
			if (DistanceTarget <= MilitaryBossAI->GetHammerShotRange())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PrimaryAttack, true);
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PatternAttack, false);
			}
			else if(DistanceTarget > MilitaryBossAI->GetHammerShotRange() && DistanceTarget <= MilitaryBossAI->patternDistance)
			{	
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PrimaryAttack, false);
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PatternAttack, false);
			}
			else if (DistanceTarget > MilitaryBossAI->patternDistance)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PatternAttack, true);
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(AMilitaryBoss_AIController::PrimaryAttack, false);
			}
		}
	}
}
