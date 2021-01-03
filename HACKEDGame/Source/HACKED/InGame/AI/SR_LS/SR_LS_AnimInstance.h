// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "SR_LS_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class HACKED_API USR_LS_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()

public:
	USR_LS_AnimInstance();

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 NewSection);
	void StopMontage();

public:
	//void NativeUpdateAnimation(float DeltaSeconds) override;
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	void SetDeadAnim(const bool& bIsDead) { IsDead = bIsDead; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rigidity,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsRigidity;

private:
	UFUNCTION()
		void AnimNotify_AttackHitCheck();

	UFUNCTION()
		void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int32 Section);

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* LS_AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dead,
		Meta = (AllowPrivateAccess = true))
		bool IsDead;

};
