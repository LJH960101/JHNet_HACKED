// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "MB_AI_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FMilitaryBossAINotifyDelegate);

/**
 * 
 */
UCLASS()
class HACKED_API UMB_AI_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()
	
public:
	UMB_AI_AnimInstance();

public:
	void NativeUpdateAnimation(float DeltaSeconds) override;

	FMilitaryBossAINotifyDelegate OnHammeringCheck;
	FMilitaryBossAINotifyDelegate OnHammeringEnd;
	FMilitaryBossAINotifyDelegate OnMachineGunCheck;
	FMilitaryBossAINotifyDelegate OnMachineGunEnd;
	FMilitaryBossAINotifyDelegate OnMissileCheck;
	FMilitaryBossAINotifyDelegate OnMissileEnd;
	FMilitaryBossAINotifyDelegate OnSlapRushCheck;
	FMilitaryBossAINotifyDelegate OnSlapRushEnd;
	FMilitaryBossAINotifyDelegate OnMBAIRigidityEnd;
	FMilitaryBossAINotifyDelegate OnMBAIBeHitEnd;

private:

	UFUNCTION()
		void AnimNotify_HammeringCheck();

	UFUNCTION()
		void AnimNotify_HammeringEnd();

	UFUNCTION()
		void AnimNotify_MachineGunCheck();

	UFUNCTION()
		void AnimNotify_MachineGunEnd();

	UFUNCTION()
		void AnimNotify_MissileCheck();

	UFUNCTION()
		void AnimNotify_MissileEnd();

	UFUNCTION()
		void AnimNotify_SlapRushCheck();

	UFUNCTION()
		void AnimNotify_SlapRushEnd();

	UFUNCTION()
		void AnimNotify_RigidityEnd();

	UFUNCTION()
		void AnimNotify_BeHitEnd();

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsHammering = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsHomingMissile = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsMachineGun = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsVortexBeam = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack)
		bool AnimIsSlapRush = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = BeHit)
		bool AnimIsBeHit = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Rigidity)
		bool AnimIsRigidity = false;



};
