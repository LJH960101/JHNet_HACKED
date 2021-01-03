// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "SR_ET_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnElectricShockCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnElectricShockFinishDelegate);
DECLARE_MULTICAST_DELEGATE(FOnElectricShockEndDelegate);
/**
 * 
 */
UCLASS()
class HACKED_API USR_ET_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()

public:
	USR_ET_AnimInstance();

public:
	void NativeUpdateAnimation(float DeltaSeconds) override;
	FOnElectricShockCheckDelegate OnElectricShockCheck;
	FOnElectricShockFinishDelegate OnElectricShockFinish;
	FOnElectricShockEndDelegate OnElectricShockEnd;
	void SetDeadAnim(const bool& isDead) { IsDead = isDead; }

private:

	UFUNCTION()
		void AnimNotify_Electric_Shock_Check();

	UFUNCTION()
		void AnimNotify_Electric_Shock_End();


	FName GetElectricAttackSectionName(int32 Section);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsElectricShock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rigidity,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsRigidity;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		UAnimInstance* ElectricAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta =
		(AllowPrivateAccess = true))
		bool IsDead;
};
