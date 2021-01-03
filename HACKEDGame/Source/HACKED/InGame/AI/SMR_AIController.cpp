// Fill out your copyright notice in the Description page of Project Settings.

// SecurityRobot , MilitaryRobot AI Controller Class
// 기본 AI 및 네임드 AI는 하나의 AIController 에서 관리해줍니다. 


#include "SMR_AIController.h"
#include "SR_LS/SecurityRobot_LS.h"
#include "SR_SLS/SecurityRobot_SLS.h"
#include "SR_ET/SecurityRobot_ET.h"
#include "SelfDestructAI/SelfDestructAI.h"
#include "MilitarySoldierAI/MilitarySoldierAI.h"
#include "MilitaryNamedAI/MilitaryNamedAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName ASMR_AIController::HomePosKey(TEXT("HomePos"));
const FName ASMR_AIController::PatrolPosKey(TEXT("PatrolPos"));
const FName ASMR_AIController::MoveToAttack(TEXT("MoveToAttack"));
const FName ASMR_AIController::TargetKey(TEXT("Target"));
const FName ASMR_AIController::AttackAllow(TEXT("AttackAllow"));
const FName ASMR_AIController::AllStop(TEXT("AllStop"));
const FName ASMR_AIController::MS_AI_LongAttack(TEXT("MS_AI_LongAttack"));
const FName ASMR_AIController::MS_AI_ShortAttack(TEXT("MS_AI_ShortAttack"));
const FName ASMR_AIController::SwapAttackState(TEXT("SwapAttackState"));
const FName ASMR_AIController::MN_AI_PhotonShot(TEXT("MN_AI_PhotonShot"));
const FName ASMR_AIController::MN_AI_RagingBlow(TEXT("MN_AI_RagingBlow"));

ASMR_AIController::ASMR_AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	// BlackBoard
	static ConstructorHelpers::FObjectFinder<UBlackboardData> Hacked_AI_BBObject(TEXT("/Game/Resources/Enemy/AI/HACKED_AI_BB.HACKED_AI_BB"));
	if (Hacked_AI_BBObject.Succeeded())
	{
		Hacked_AI_BB = Hacked_AI_BBObject.Object;
	}

	// Behavior Tree
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> MS_AI_BTObject(TEXT("/Game/Resources/Enemy/AI/MilitarySoldierAI/NEW/MilitarySoldierAI_BT.MilitarySoldierAI_BT"));
	if (MS_AI_BTObject.Succeeded())
	{
		MSAI_BTAsset = MS_AI_BTObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> MN_AI_BTObject(TEXT("/Game/Resources/Enemy/AI/MilitaryNamedAI/MilitaryNamedAI_BT.MilitaryNamedAI_BT"));
	if (MN_AI_BTObject.Succeeded())
	{
		MNAI_BTAsset = MN_AI_BTObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SD_AI_BTObject(TEXT("/Game/Resources/Enemy/AI/SelfDestruct_AI/SelfDestruct_AI_BT.SelfDestruct_AI_BT"));
	{
		SDAI_BTAsset = SD_AI_BTObject.Object;
	}

	// Old BehaviorTree(HACKED 초기에 사용하던 AI) 
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SRLS_BTObject(TEXT("/Game/Resources/Enemy/AI/SecurityRobot_LS/SecurityRobot_LS_BT.SecurityRobot_LS_BT"));
	if (SRLS_BTObject.Succeeded())
	{
		SRLS_BTAsset = SRLS_BTObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SR_SLS_BTObject(TEXT("/Game/Resources/Enemy/AI/SecurityRobot_SLS/SecurityRobot_SLS_BT.SecurityRobot_SLS_BT"));
	if (SR_SLS_BTObject.Succeeded())
	{
		SRSLS_BTAsset = SR_SLS_BTObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SRET_Object(TEXT("/Game/Resources/Enemy/AI/SecurityRobot_ET/SecurityRobot_ET_BT.SecurityRobot_ET_BT"));
	if (SRET_Object.Succeeded())
	{
		SRET_BTAsset = SRET_Object.Object;
	}
}

void ASMR_AIController::OnPossess(APawn * InPawn)
{
	Super::OnPossess(InPawn);

	// Set crowd moving (AI의 뭉침 현상을 방지함) 
	UActorComponent* acCfCP = GetComponentByClass(UCrowdFollowingComponent::StaticClass());
	if (acCfCP != nullptr) {
		UCrowdFollowingComponent* cfCp = Cast<UCrowdFollowingComponent>(acCfCP);
		if (cfCp != nullptr) {
			cfCp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
		}
	}

	if (UseBlackboard(Hacked_AI_BB, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (Cast<ASecurityRobot_LS>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(SRLS_BTAsset))
				LOG(Error, "SMR_AIController couldn't run SRLS_Behevaior Tree!");
		}

		else if (Cast<ASecurityRobot_SLS>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(SRSLS_BTAsset))
				LOG(Error, "SMR_AIController couldn't run SRSLS_BTAsset Tree!");
		}

		else if (Cast<ASecurityRobot_ET>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(SRET_BTAsset))
				LOG(Error, "SMR_AIController couldn't run SRET_Behavaior Tree");
		}

		else if (Cast<AMilitarySoldierAI>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(MSAI_BTAsset))
				LOG(Error, "SMR_AIController couldn't run MSAI_Behavaior Tree");
		}
		else if (Cast<AMilitaryNamedAI>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(MNAI_BTAsset))
				LOG(Error, "SMR_AIController couldn't run MNAI_Behavaior Tree");
		}
		else if (Cast<ASelfDestructAI>(GetPawn()) != nullptr)
		{
			if (!RunBehaviorTree(SDAI_BTAsset))
				LOG(Error, "SMR_AIController couldn't run SDAI_Behavior Tree");
		}
	}
}


