// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"

#include "PathIndicator.generated.h"

UCLASS()
class HACKED_API APathIndicator : public AActor
{
	GENERATED_BODY()
public:
	APathIndicator();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Start();
	void Stop();
private:
	UPROPERTY(EditAnywhere)
	class USplineComponent* splineComp;
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* indicatorEffectComp;
	UPROPERTY(EditAnywhere)
	UParticleSystem* indicatorEffect;
	float splineLength = 0.0f;
	UPROPERTY(EditAnywhere)
	float movingSpeedPerSec = 3000.0f;
	float currentDistance = 0.0f;
	bool bIsRunning = false;
	void Move();
};
