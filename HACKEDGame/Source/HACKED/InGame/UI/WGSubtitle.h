// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGSubtitle.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UWGSubtitle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Subtitle")
	void ShowSubtitle(const FString& subtitle, const float& time);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Subtitle")
	void StopSubtitle();
};
