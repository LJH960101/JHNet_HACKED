// Fill out your copyright notice in the Description page of Project Settings.

#include "PathIndicator.h"
#include "Components/SplineComponent.h"

APathIndicator::APathIndicator()
{
	PrimaryActorTick.bCanEverTick = true;
	
	splineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SPLINE COMPONENT"));
	indicatorEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("INDICATOR COMP"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem>PS_INDICATOR(TEXT("ParticleSystem'/Game/Resources/Object/PathIndicator/PS_PathIndicator.PS_PathIndicator'"));
	if (PS_INDICATOR.Succeeded())
	{
		indicatorEffect = PS_INDICATOR.Object;
		indicatorEffectComp->SetTemplate(indicatorEffect);
	}
}

void APathIndicator::BeginPlay()
{
	Super::BeginPlay();
	splineLength = splineComp->GetSplineLength();
	Start();
}

void APathIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsRunning)
		return;

	if (currentDistance > splineLength)
	{
		currentDistance = 0.0f;
		indicatorEffectComp->Activate();
	}
	
	if (currentDistance >= splineLength - 100.0f)
		indicatorEffectComp->Deactivate();

	currentDistance += GetWorld()->GetDeltaSeconds()* movingSpeedPerSec;
	Move();
}

void APathIndicator::Start()
{
	bIsRunning = true;
	indicatorEffectComp->Activate();
}

void APathIndicator::Stop()
{
	bIsRunning = false;
	indicatorEffectComp->Deactivate();
}

void APathIndicator::Move()
{
	FVector splineLoc = splineComp->GetWorldLocationAtDistanceAlongSpline(currentDistance);
	FRotator splineRot = splineComp->GetWorldRotationAtDistanceAlongSpline(currentDistance);
	indicatorEffectComp->SetWorldLocationAndRotation(splineLoc, splineRot);
}

