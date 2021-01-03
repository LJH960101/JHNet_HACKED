// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "AIController.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "SMR_AIController.generated.h"

/**
 * 
 */

UCLASS()
class HACKED_API ASMR_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ASMR_AIController(const FObjectInitializer& ObjectInitializer);
	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName MoveToAttack;
	static const FName TargetKey;
	static const FName AttackAllow;
	static const FName AllStop;
	static const FName MS_AI_LongAttack;
	static const FName MS_AI_ShortAttack;
	static const FName SwapAttackState;
	static const FName MN_AI_PhotonShot;
	static const FName MN_AI_RagingBlow;

private:
	ASMR_AIController();
	UPROPERTY()
		class UBlackboardData* Hacked_AI_BB;

public:

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* MSAI_BTAsset;

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* MNAI_BTAsset;
	
	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* SDAI_BTAsset;

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* SRLS_BTAsset;

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* SRSLS_BTAsset;
	
	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	class UBehaviorTree* SRET_BTAsset;

};
