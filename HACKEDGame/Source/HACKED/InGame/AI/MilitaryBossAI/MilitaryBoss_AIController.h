// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "AIController.h"
#include "MilitaryBoss_AIController.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API AMilitaryBoss_AIController : public AAIController
{
	GENERATED_BODY()


public:
	AMilitaryBoss_AIController(const FObjectInitializer& ObjectInitializer);

	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePos;
	static const FName PatrolPos;
	static const FName PrimaryAttack;
	static const FName PatternAttack;
	static const FName SlapRush;
	static const FName TargetPlayer;
	


private:
	UPROPERTY()
		class UBlackboardData* MilitaryBossAI_BB;

public:

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* MBAI_BT;

	
};
