// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WGDamageOutput.generated.h"

/**
 * 
 */
UCLASS()
class HACKED_API UWGDamageOutput : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "AI")
	AActor* owner;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetOwner(AActor* target) {
		owner = target;
	}
};
