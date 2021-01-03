// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MBAI_Attack.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTTask_MBAI_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MBAI_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void HammerShotEnd(UBehaviorTreeComponent* OwnerComp);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
