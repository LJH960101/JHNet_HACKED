// Fill out your copyright notice in the Description page of Project Settings.


#include "MB_AI_AnimInstance.h"


UMB_AI_AnimInstance::UMB_AI_AnimInstance()
{

}

void UMB_AI_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;
}

void UMB_AI_AnimInstance::AnimNotify_HammeringCheck()
{
	OnHammeringCheck.Broadcast();
}

void UMB_AI_AnimInstance::AnimNotify_HammeringEnd()
{
	OnHammeringEnd.Broadcast();
	AnimIsHammering = false;
}

void UMB_AI_AnimInstance::AnimNotify_MachineGunCheck()
{
	OnMachineGunCheck.Broadcast();
}

void UMB_AI_AnimInstance::AnimNotify_MachineGunEnd()
{
	OnMachineGunEnd.Broadcast();
	AnimIsMachineGun = false;
}

void UMB_AI_AnimInstance::AnimNotify_MissileCheck()
{
	OnMissileCheck.Broadcast();
}

void UMB_AI_AnimInstance::AnimNotify_MissileEnd()
{
	OnMissileEnd.Broadcast();
	AnimIsHomingMissile = false;
}

void UMB_AI_AnimInstance::AnimNotify_SlapRushCheck()
{
	OnSlapRushCheck.Broadcast();
}

void UMB_AI_AnimInstance::AnimNotify_SlapRushEnd()
{
	OnSlapRushEnd.Broadcast();
	AnimIsSlapRush = false;
}

void UMB_AI_AnimInstance::AnimNotify_RigidityEnd()
{
	OnMBAIRigidityEnd.Broadcast();
	AnimIsRigidity = false;
}

void UMB_AI_AnimInstance::AnimNotify_BeHitEnd()
{
	OnMBAIBeHitEnd.Broadcast();
	AnimIsBeHit = false;
}
