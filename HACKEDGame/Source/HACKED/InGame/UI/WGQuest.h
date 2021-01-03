// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "Blueprint/UserWidget.h"
#include "WGQuest.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UWGQuest : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void RenewalWidget(const FString& data);
};
