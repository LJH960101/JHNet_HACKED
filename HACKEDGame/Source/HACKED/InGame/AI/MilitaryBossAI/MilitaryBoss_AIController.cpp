// Fill out your copyright notice in the Description page of Project Settings.


#include "MilitaryBoss_AIController.h"
#include "MilitaryBossAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AMilitaryBoss_AIController::HomePos(TEXT("HomePos"));
const FName AMilitaryBoss_AIController::PatrolPos(TEXT("PatrolPos"));
const FName AMilitaryBoss_AIController::PrimaryAttack(TEXT("Primary Attack"));
const FName AMilitaryBoss_AIController::PatternAttack(TEXT("Pattern Attack"));
const FName AMilitaryBoss_AIController::SlapRush(TEXT("Slap Rush"));
const FName AMilitaryBoss_AIController::TargetPlayer(TEXT("Target Player"));

AMilitaryBoss_AIController::AMilitaryBoss_AIController(const FObjectInitializer& ObjectInitializer)
{
	// BlackBoard
	static ConstructorHelpers::FObjectFinder<UBlackboardData> MilitaryBossAI_BBObject(TEXT("/Game/Resources/Enemy/AI/MilitaryBossAI/MilitaryBossAI_BB.MilitaryBossAI_BB"));
	if (MilitaryBossAI_BBObject.Succeeded())
	{
		MilitaryBossAI_BB = MilitaryBossAI_BBObject.Object;
	}

	// Behavior Tree
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> MB_AI_BTObject(TEXT("/Game/Resources/Enemy/AI/MilitaryBossAI/MilitaryBossAI_BT.MilitaryBossAI_BT"));
	if (MB_AI_BTObject.Succeeded())
	{
		MBAI_BT = MB_AI_BTObject.Object;
	}
}

void AMilitaryBoss_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(MilitaryBossAI_BB, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePos, InPawn->GetActorLocation());
		if (Cast<AMilitaryBossAI>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(MBAI_BT))
				LOG(Error, "Couldn't run MBAI_Behavior Tree !");
		}
	}
}
