// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "../AIAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "../AIAnimInstance.h"
#include "SD_AI_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FSelfDestructAINotifyDelegate);


/**
 * 
 */
UCLASS()
class HACKED_API USD_AI_AnimInstance : public UAIAnimInstance
{
	GENERATED_BODY()

public:

	USD_AI_AnimInstance();

public:
	void NativeUpdateAnimation(float DeltaSeconds) override;
	FSelfDestructAINotifyDelegate OnSelfDestructDamaging;


private:

	UFUNCTION()
		void AnimNotify_SelfDestructCheck();


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dead,
		Meta = (AllowPrivateAccess = true))
		bool IsDead;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfDestruct,
		Meta = (AllowPrivateAccess = true))
		bool AnimIsSelfDestruct;




	
};
