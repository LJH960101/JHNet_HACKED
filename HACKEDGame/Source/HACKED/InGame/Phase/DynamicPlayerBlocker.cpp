// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicPlayerBlocker.h"

ADynamicPlayerBlocker::ADynamicPlayerBlocker()
{
 	PrimaryActorTick.bCanEverTick = true;
	
	blocker = CreateDefaultSubobject<UBoxComponent>(TEXT("BLOCKER"));
	
	RootComponent = blocker;
}

void ADynamicPlayerBlocker::BeginPlay()
{
	Super::BeginPlay();
	CLOSE();
}

void ADynamicPlayerBlocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADynamicPlayerBlocker::OPEN()
{
	blocker->SetCollisionProfileName(FName("NoCollision"));
}

void ADynamicPlayerBlocker::CLOSE()
{
	blocker->SetCollisionProfileName(FName("PlasmaDoor"));
}
