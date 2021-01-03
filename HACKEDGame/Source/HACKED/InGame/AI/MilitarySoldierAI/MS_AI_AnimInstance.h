// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MS_AI_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FMilitarySoldierAINotifyDelegate);


/**
 * 
 */
UCLASS()
class HACKED_API UMS_AI_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()

public:
	UMS_AI_AnimInstance();

public:
	void PlayBladeMontage();
	void JumpToAttackMontageSection(int32 NewSection);
	void StopMontage();

public:
	void NativeUpdateAnimation(float DeltaSeconds) override; 
	FMilitarySoldierAINotifyDelegate OnLaserShockCheck; // 데미지 판정을 위한 델리게이트 
	FMilitarySoldierAINotifyDelegate OnLaserShockEnd; 
	FMilitarySoldierAINotifyDelegate OnGetPlayerPosCheck;
	FMilitarySoldierAINotifyDelegate OnBladeShockCheck;
	FMilitarySoldierAINotifyDelegate OnBladeShockEnd;
	FMilitarySoldierAINotifyDelegate OnRigidityEnd;
	FMilitarySoldierAINotifyDelegate OnMSAIBeHitEnd;


private:

	UFUNCTION()
		void AnimNotify_LaserShockCheck();

	UFUNCTION()
		void AnimNotify_LaserShockEnd();

	UFUNCTION()
		void AnimNotify_BladeShockCheck();

	UFUNCTION()
		void AnimNotify_GetPlayerPos();

	UFUNCTION()
		void AnimNotify_BladeShockEnd();

	UFUNCTION()
		void AnimNotify_BeHitEnd();

	UFUNCTION()
		void AnimNotify_RigidityEnd();

	FName GetBladeMontageSectionName(int32 Section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta =
		(AllowPrivateAccess = true))
		bool IsDead;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage * MSAI_Montage;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsLaserShock;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsBladeShock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rigidity,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsRigidity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rigidity,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsBeHit;



};
