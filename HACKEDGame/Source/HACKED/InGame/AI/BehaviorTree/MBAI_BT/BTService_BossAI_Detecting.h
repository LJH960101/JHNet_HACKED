// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTService.h"
#include "BTService_BossAI_Detecting.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTService_BossAI_Detecting : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_BossAI_Detecting();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:

	AMilitaryBossAI* MilitaryBossAI;

	AHACKEDCharacter* targetPlayer;

	TTuple<AActor*, float> nearestPlayer;

private:
	int _moveStateCount = 0;

};
