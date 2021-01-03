// Fill out your copyright notice in the Description page of Project Settings.


#include "MN_AI_AnimInstance.h"

UMN_AI_AnimInstance::UMN_AI_AnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MNAI_MONTAGE(TEXT("/Game/Resources/Enemy/AI/MilitaryNamedAI/MilitaryNamedAI_SK_Skeleton_Montage.MilitaryNamedAI_SK_Skeleton_Montage"));
	if (MNAI_MONTAGE.Succeeded())
	{
		MNAI_Montage = MNAI_MONTAGE.Object;
	}

	AnimIsBeHit = false;
	AnimIsRigidity = false;
	AnimIsPhotonShot = false;
	AnimIsRagingBlow = false;
	IsDead = false;
}

void UMN_AI_AnimInstance::PlayRagingBlowMontage()
{
	Montage_Play(MNAI_Montage, 1.0f);
}

void UMN_AI_AnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	CHECK(Montage_Play(MNAI_Montage));
	Montage_JumpToSection(GetRagingBlowMontageSectionName(NewSection), MNAI_Montage);
}

void UMN_AI_AnimInstance::StopMontage()
{
	Montage_Stop(0.1f);
}

void UMN_AI_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;
}

void UMN_AI_AnimInstance::AnimNotify_RagingBlowCheck()
{
	OnRagingBlowCheck.Broadcast();
}

void UMN_AI_AnimInstance::AnimNotify_PhotonShotCheck()
{
	OnPhotonShotCheck.Broadcast();
}

void UMN_AI_AnimInstance::AnimNotify_PhotonShotEnd()
{
	OnPhotonShotEnd.Broadcast();
	AnimIsPhotonShot = false;
}

void UMN_AI_AnimInstance::AnimNotify_RigidityEnd()
{
	OnMNAIRigidityEnd.Broadcast();
	AnimIsRigidity = false;
}

void UMN_AI_AnimInstance::AnimNotify_BeHitEnd()
{
	OnMNAIBeHitEnd.Broadcast();
	AnimIsBeHit = false;
}

FName UMN_AI_AnimInstance::GetRagingBlowMontageSectionName(int32 Section)
{
	CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 2), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
