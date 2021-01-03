// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "BehaviorTree/BTTaskNode.h"
#include "InGame/AI/HACKED_AI.h"
#include "BTTask_AI_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTTask_AI_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AI_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	
};
