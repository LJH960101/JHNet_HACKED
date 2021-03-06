// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "SR_SLS_AnimInstance.h"

USR_SLS_AnimInstance::USR_SLS_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> SLS_ATTACKMONTAGE(TEXT("AnimMontage'/Game/Resources/Enemy/AI/SecurityRobot_SLS/SecurityRobot_SLS_Test_Montage.SecurityRobot_SLS_Test_Montage'"));
	if (SLS_ATTACKMONTAGE.Succeeded())
	{
		SLSAttackMontage = SLS_ATTACKMONTAGE.Object;
	}

	IsDead = false;
}

void USR_SLS_AnimInstance::PlaySLSAttackMontage()
{
	Montage_Play(SLSAttackMontage, 1.0f);
}

void USR_SLS_AnimInstance::JumpToSLSAttackMontageSection(int NewSection)
{
	CHECK(Montage_IsPlaying(SLSAttackMontage));
	Montage_JumpToSection(GetSLSAttackMontageSectionName(NewSection), SLSAttackMontage);
}

void USR_SLS_AnimInstance::AnimNotify_AttackCheck()
{
	LOG_S(Warning);
	OnSLSAttackHitCheck.Broadcast();
}

void USR_SLS_AnimInstance::AnimNotify_NextAttackCheck()
{
	OnSLSNextAttackCheck.Broadcast();
}

FName USR_SLS_AnimInstance::GetSLSAttackMontageSectionName(int32 Section)
{
	CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("SLSAttack%d"), Section));
}
