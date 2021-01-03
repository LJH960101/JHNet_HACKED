// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AI_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTTask_AI_Patrol : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_AI_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
