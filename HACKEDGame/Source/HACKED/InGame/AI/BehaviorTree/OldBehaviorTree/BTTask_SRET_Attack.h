// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SRET_Attack.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTTask_SRET_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SRET_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
