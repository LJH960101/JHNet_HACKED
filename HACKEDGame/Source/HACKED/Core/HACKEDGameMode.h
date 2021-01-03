// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "Core/JHNETGameMode.h"
#include "HACKEDGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API AHACKEDGameMode : public AJHNETGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, Category = ExecFunctions)
	void AIAllDie();

	UFUNCTION(Exec, Category = ExecFunctions)
	void SkipTutorial();

	UFUNCTION(Exec, Category = ExecFunctions)
	void GoToBoss();

	UFUNCTION(Exec, Category = ExecFunction)
	void BossEasyMode();

	UFUNCTION(Exec, Category = ExecFunction)
	void BossNormalMode();

	UFUNCTION(Exec, Category = ExecFunction)
	void BossHardMode();
};
