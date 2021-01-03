// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "JHNET.h"
#include "GameFramework/GameMode.h"
#include "JHNETGameMode.generated.h"

/**
 * 
 */
UCLASS()
class JHNET_API AJHNETGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, Category = ExecFunctions)
	void DisconnectTest();

	UFUNCTION(Exec, Category = ExecFunctions)
	void RecvErrorTest();

	UFUNCTION(Exec, Category = ExecFunctions)
	void IsServer();

	UFUNCTION(Exec, Category = ExecFunctions)
	void ForceReconnect();

	UFUNCTION(Exec, Category = ExecFunctions)
	void ForeceEndGame();
};
