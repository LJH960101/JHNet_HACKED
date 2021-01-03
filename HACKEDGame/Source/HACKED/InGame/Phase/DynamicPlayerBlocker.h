// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "DynamicPlayerBlocker.generated.h"

UCLASS()
class HACKED_API ADynamicPlayerBlocker : public AActor
{
	GENERATED_BODY()
	
public:	
	ADynamicPlayerBlocker();
	
	virtual void Tick(float DeltaTime) override;

	void OPEN();
	void CLOSE();

protected:
	virtual void BeginPlay() override;

private:
	UBoxComponent* blocker;
};
