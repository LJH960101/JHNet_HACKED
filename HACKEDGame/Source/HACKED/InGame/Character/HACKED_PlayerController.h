// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/PlayerController.h"
#include "HACKED_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API AHACKED_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHACKED_PlayerController();	
	virtual void OnPossess(APawn* aPawn);
};
