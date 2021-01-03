// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/AI/BehaviorTree/BTService_HackedAI_Detecting.h"
#include "BehaviorTree/BTService.h"
#include "BTService_NamedAI_Detecting.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTService_NamedAI_Detecting : public UBTService_HackedAI_Detecting
{
	GENERATED_BODY()

public:
	UBTService_NamedAI_Detecting();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
