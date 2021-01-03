// Fill out your copyright notice in the Description page of Project Settings.

// SecurityRobot_LS AnimInstance

#include "SR_LS_AnimInstance.h"

USR_LS_AnimInstance::USR_LS_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> LS_ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Resources/Enemy/AI/SecurityRobot_LS/Saver_riggiedmesh_Skeleton_Montage.Saver_riggiedmesh_Skeleton_Montage'"));
		if (LS_ATTACK_MONTAGE.Succeeded())
		{
			LS_AttackMontage = LS_ATTACK_MONTAGE.Object;
		}
	IsDead = false;
}

void USR_LS_AnimInstance::PlayAttackMontage()
{
	Montage_Play(LS_AttackMontage, 1.0f);
}

void USR_LS_AnimInstance::StopMontage()
{
	Montage_Stop(0.1f);
}

void USR_LS_AnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	CHECK(Montage_IsPlaying(LS_AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), LS_AttackMontage);
}

//void USR_LS_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
//{
//}

void USR_LS_AnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void USR_LS_AnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName USR_LS_AnimInstance::GetAttackMontageSectionName(int32 Section)
{
	CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 2), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
