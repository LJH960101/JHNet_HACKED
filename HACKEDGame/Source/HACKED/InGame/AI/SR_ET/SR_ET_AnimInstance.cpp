// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_ET_AnimInstance.h"
#include "SecurityRobot_ET.h"

USR_ET_AnimInstance::USR_ET_AnimInstance()
{
	AnimIsElectricShock = false;
	IsDead = false;
}

void USR_ET_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;
}


void USR_ET_AnimInstance::AnimNotify_Electric_Shock_Check()
{
	OnElectricShockCheck.Broadcast();
}

void USR_ET_AnimInstance::AnimNotify_Electric_Shock_End()
{
	OnElectricShockFinish.Broadcast();
	OnElectricShockEnd.Broadcast();
	AnimIsElectricShock = false;
}


FName USR_ET_AnimInstance::GetElectricAttackSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Electric%d"), Section));
}
