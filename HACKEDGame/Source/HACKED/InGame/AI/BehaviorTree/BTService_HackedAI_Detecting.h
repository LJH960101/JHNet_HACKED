// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/AI/SMR_AIController.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "InGame/AI/HACKED_AI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTService.h"
#include "BTService_HackedAI_Detecting.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UBTService_HackedAI_Detecting : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_HackedAI_Detecting();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	float _detectRadius;

public:

	TTuple<AActor*, float> nearestPlayer;

	AHACKED_AI* HACKED_AI;
};
