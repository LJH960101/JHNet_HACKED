// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "Animation/AnimInstance.h"
#include "HACKED_ESPER.h"
#include "../HackedCharacterAnimInstance.h"
#include "Esper_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FVoidNotifyDelegate);

UCLASS()
class HACKED_API UEsper_AnimInstance : public UHackedCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UEsper_AnimInstance();

public:
	FVoidNotifyDelegate OnAttackCheck;

	FVoidNotifyDelegate OnPsychicForceCheck;
	FVoidNotifyDelegate OnPsychicForceEnd;

	FVoidNotifyDelegate OnPsychicDropCheck;
	FVoidNotifyDelegate OnPsychicDropDamaging;
	FVoidNotifyDelegate OnPsychicDropEnd;

	FVoidNotifyDelegate OnPsychicShockWave;
	FVoidNotifyDelegate ShockWaveDamagingEnd;
	FVoidNotifyDelegate OnPsychicShockWaveAnimEnd;

	FVoidNotifyDelegate OnPsychicShieldCheck;
	FVoidNotifyDelegate OnPsychicShieldAnimEnd;

	FVoidNotifyDelegate OnPsychicOverDriveCheck;
	FVoidNotifyDelegate OnPsychicOverDriveCamReturn;
	FVoidNotifyDelegate OnPsychicOverDriveEndAction;
	FVoidNotifyDelegate OnPsychicOverDriveEnd;

	FVoidNotifyDelegate OnEsperSelfHealingEnd;
	FVoidNotifyDelegate OnEsperDashEnd;

	FVoidNotifyDelegate EsperDieEnd;

private:
	UFUNCTION()
		void AnimNotify_AttackCheck();

	UFUNCTION()
		void AnimNotify_PsychicForceCheck();

	UFUNCTION()
		void AnimNotify_PsychicForceEnd();

	UFUNCTION()
		void AnimNotify_PsychicDropCheck();

	UFUNCTION()
		void AnimNotify_PsychicDropDamaging();

	UFUNCTION()
		void AnimNotify_PsychicDropEnd();

	UFUNCTION()
		void AnimNotify_PsychicShockWaveCheck();

	UFUNCTION()
		void AnimNotify_PsychicShockWaveEnd();

	UFUNCTION()
		void AnimNotify_PsychicShockWaveAnimEnd();

	UFUNCTION()
		void AnimNotify_PsychicShieldCheck();

	UFUNCTION()
		void AnimNotify_PsychicShieldAnimEnd();

	UFUNCTION()
		void AnimNotify_PsychicOverDriveCheck();

	UFUNCTION()
		void AnimNotify_PsychicOverDriveCamReturn();

	UFUNCTION()
		void AnimNotify_PsychicOverDriveEndAction();

	UFUNCTION()
		void AnimNotify_PsychicOverDriveEnd();

	UFUNCTION()
		void AnimNotify_EsperSelfHealingEnd();

	UFUNCTION()
		void AnimNotify_EsperDashEnd();

	UFUNCTION()
		void AnimNotify_EsperDie();


	FName GetAttackMontageSectionName(int32 Section);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
		bool AnimIsPsychicDash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
		bool AnimIsPsychicForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
		bool AnimIsPsychicDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
		bool AnimIsPsychicShockWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
		bool AnimIsPsychicShield;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill)
		bool AnimIsPsychicOverDrive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfHealingState)
		bool AnimIsEsperSelfHealing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfHealingState)
		bool AnimIsEsperSelfHealingFail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfHealingState)
		bool AnimIsEsperDieProcess;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
		int currentCombo;
};
