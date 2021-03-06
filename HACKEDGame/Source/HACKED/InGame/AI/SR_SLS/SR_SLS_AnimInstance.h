// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "SR_SLS_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSLSNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSLSAttackHitCheckDelegate);

/**
 * 
 */
UCLASS()
class HACKED_API USR_SLS_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()
	
public:

	USR_SLS_AnimInstance();

	void SetDeadAnim(const bool& bIsDead) { IsDead = bIsDead; }

public:
	void PlaySLSAttackMontage();
	void JumpToSLSAttackMontageSection(int NewSection);


public:
	FOnSLSNextAttackCheckDelegate OnSLSNextAttackCheck;
	FOnSLSAttackHitCheckDelegate OnSLSAttackHitCheck;
	
private:
	UFUNCTION()
	void AnimNotify_AttackCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetSLSAttackMontageSectionName(int32 Section);
private:

	UPROPERTY()
	class USR_SLS_AnimInstance* SLS_Anim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dead,
		Meta = (AllowPrivateAccess = true))
		bool IsDead;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		UAnimMontage* SLSAttackMontage;
	
};
