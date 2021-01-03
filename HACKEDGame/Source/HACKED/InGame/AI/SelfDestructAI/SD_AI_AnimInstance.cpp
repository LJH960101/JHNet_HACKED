// Fill out your copyright notice in the Description page of Project Settings.


#include "SD_AI_AnimInstance.h"

USD_AI_AnimInstance::USD_AI_AnimInstance()
{
	IsDead = false;
	AnimIsSelfDestruct = false;
}

void USD_AI_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;
}

void USD_AI_AnimInstance::AnimNotify_SelfDestructCheck()
{
	OnSelfDestructDamaging.Broadcast();
	AnimIsSelfDestruct = false;
}
