// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "MN_AI_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FMilitaryNamedAINotifyDelegate);

/**
 * 
 */
UCLASS()
class HACKED_API UMN_AI_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()

public:
	UMN_AI_AnimInstance();

public:
	void PlayRagingBlowMontage();
	void JumpToAttackMontageSection(int32 NewSection);
	void StopMontage();

public:
	void NativeUpdateAnimation(float DeltaSeconds) override;
	FMilitaryNamedAINotifyDelegate OnRagingBlowCheck;
	FMilitaryNamedAINotifyDelegate OnPhotonShotCheck;
	FMilitaryNamedAINotifyDelegate OnPhotonShotEnd;
	FMilitaryNamedAINotifyDelegate OnMNAIRigidityEnd;
	FMilitaryNamedAINotifyDelegate OnMNAIBeHitEnd;

private:

	UFUNCTION()
		void AnimNotify_RagingBlowCheck();

	UFUNCTION()
		void AnimNotify_PhotonShotCheck();

	UFUNCTION()
		void AnimNotify_PhotonShotEnd();

	UFUNCTION()
		void AnimNotify_RigidityEnd();

	UFUNCTION()
		void AnimNotify_BeHitEnd();



	FName GetRagingBlowMontageSectionName(int32 Section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta =
		(AllowPrivateAccess = true))
		bool IsDead;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage * MNAI_Montage;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsPhotonShot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsRagingBlow;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = BeHit)
		bool AnimIsBeHit;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Rigidity)
		bool AnimIsRigidity;

};
