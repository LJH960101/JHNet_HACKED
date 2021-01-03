// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AI_MoveToAttack.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTTask_AI_MoveToAttack : public UBTTaskNode
{
	GENERATED_BODY()
	

public:
	UBTTask_AI_MoveToAttack();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
