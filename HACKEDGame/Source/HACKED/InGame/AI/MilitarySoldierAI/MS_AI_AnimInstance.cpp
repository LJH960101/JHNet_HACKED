// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_AI_AnimInstance.h"

UMS_AI_AnimInstance::UMS_AI_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MSAI_MONTAGE(TEXT("/Game/Resources/Enemy/AI/MilitarySoldierAI/NEW/MilitarySoldierAI_Montage.MilitarySoldierAI_Montage"));
	if (MSAI_MONTAGE.Succeeded())
	{
		MSAI_Montage = MSAI_MONTAGE.Object;
	}

	AnimIsLaserShock = false;
	AnimIsBladeShock = false;
	IsDead = false;
}

void UMS_AI_AnimInstance::PlayBladeMontage()
{
	Montage_Play(MSAI_Montage, 1.0f);
}

void UMS_AI_AnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	CHECK(Montage_Play(MSAI_Montage));
	Montage_JumpToSection(GetBladeMontageSectionName(NewSection), MSAI_Montage);
}

void UMS_AI_AnimInstance::StopMontage()
{
	Montage_Stop(0.1f);
}

void UMS_AI_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;
}

void UMS_AI_AnimInstance::AnimNotify_LaserShockCheck()
{
	OnLaserShockCheck.Broadcast();
}

void UMS_AI_AnimInstance::AnimNotify_LaserShockEnd()
{
	OnLaserShockEnd.Broadcast();
	AnimIsLaserShock = false;
}

void UMS_AI_AnimInstance::AnimNotify_BladeShockCheck()
{
	OnBladeShockCheck.Broadcast();
}

void UMS_AI_AnimInstance::AnimNotify_GetPlayerPos()
{
	OnGetPlayerPosCheck.Broadcast();
}

void UMS_AI_AnimInstance::AnimNotify_BladeShockEnd()
{
	OnBladeShockEnd.Broadcast();
	AnimIsBladeShock = false;
}

void UMS_AI_AnimInstance::AnimNotify_BeHitEnd()
{
	OnMSAIBeHitEnd.Broadcast();
	AnimIsBeHit = false;
}

void UMS_AI_AnimInstance::AnimNotify_RigidityEnd()
{
	OnRigidityEnd.Broadcast();
	AnimIsRigidity = false;
}

FName UMS_AI_AnimInstance::GetBladeMontageSectionName(int32 Section)
{
	CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 2), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

