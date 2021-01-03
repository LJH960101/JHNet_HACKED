// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGFade.generated.h"

/**
 * 
 */
UCLASS()
class JHNET_API UWGFade : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Fade)
	void FadeIn(float Time);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Fade)
	void FadeOut(float Time);
};
