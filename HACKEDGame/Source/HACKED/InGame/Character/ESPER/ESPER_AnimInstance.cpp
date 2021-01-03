// Fill out your copyright notice in the Description page of Project Settings.

#include "Esper_AnimInstance.h"

UEsper_AnimInstance::UEsper_AnimInstance()
{
	AnimIsPsychicShockWave = false;
	currentCombo = 0;
}

void UEsper_AnimInstance::AnimNotify_AttackCheck()
{
	OnAttackCheck.Broadcast();
}

void UEsper_AnimInstance::AnimNotify_PsychicForceCheck()
{
	OnPsychicForceCheck.Broadcast();
	AnimIsPsychicForce = true;
}

void UEsper_AnimInstance::AnimNotify_PsychicForceEnd()
{
	OnPsychicForceEnd.Broadcast();
	AnimIsPsychicForce = false;
}

void UEsper_AnimInstance::AnimNotify_PsychicDropCheck()
{
	OnPsychicDropCheck.Broadcast();
	AnimIsPsychicDrop = true;
}

void UEsper_AnimInstance::AnimNotify_PsychicDropDamaging()
{
	OnPsychicDropDamaging.Broadcast();
}

void UEsper_AnimInstance::AnimNotify_PsychicDropEnd()
{
	OnPsychicDropEnd.Broadcast();
	AnimIsPsychicDrop = false;
}

void UEsper_AnimInstance::AnimNotify_PsychicShockWaveCheck()
{
	OnPsychicShockWave.Broadcast();
	AnimIsPsychicShockWave = true;
	
}

void UEsper_AnimInstance::AnimNotify_PsychicShockWaveEnd()
{
	ShockWaveDamagingEnd.Broadcast();	
}

void UEsper_AnimInstance::AnimNotify_PsychicShockWaveAnimEnd()
{
	OnPsychicShockWaveAnimEnd.Broadcast();
	AnimIsPsychicShockWave = false;
}

void UEsper_AnimInstance::AnimNotify_PsychicShieldCheck()
{
	OnPsychicShieldCheck.Broadcast();

}

void UEsper_AnimInstance::AnimNotify_PsychicShieldAnimEnd()
{
	OnPsychicShieldAnimEnd.Broadcast();
	AnimIsPsychicShield = false;
}

void UEsper_AnimInstance::AnimNotify_PsychicOverDriveCheck()
{
	OnPsychicOverDriveCheck.Broadcast();
	AnimIsPsychicOverDrive = true;
}

void UEsper_AnimInstance::AnimNotify_PsychicOverDriveCamReturn()
{
	OnPsychicOverDriveCamReturn.Broadcast();
}

void UEsper_AnimInstance::AnimNotify_PsychicOverDriveEndAction()
{
	OnPsychicOverDriveEndAction.Broadcast();
}

void UEsper_AnimInstance::AnimNotify_PsychicOverDriveEnd()
{
	OnPsychicOverDriveEnd.Broadcast();
	AnimIsPsychicOverDrive = false;
}

void UEsper_AnimInstance::AnimNotify_EsperSelfHealingEnd()
{
	OnEsperSelfHealingEnd.Broadcast();
	AnimIsEsperSelfHealing = false;
}

void UEsper_AnimInstance::AnimNotify_EsperDashEnd()
{
	OnEsperDashEnd.Broadcast();
	AnimIsPsychicDash = false;
}

void UEsper_AnimInstance::AnimNotify_EsperDie()
{
	EsperDieEnd.Broadcast();
}

FName UEsper_AnimInstance::GetAttackMontageSectionName(int32 Section)
{
	LOG(Warning, "%s", *FString::Printf(TEXT("Attack%d"), Section));
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}