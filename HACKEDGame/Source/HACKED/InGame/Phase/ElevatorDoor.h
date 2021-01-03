// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "ElevatorDoor.generated.h"

UCLASS()
class HACKED_API AElevatorDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevatorDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OPEN();

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* elevatorDoor;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* elevatorBase;
	UPROPERTY(VisibleAnywhere)
	USoundBase* openSound;
	UPROPERTY(VisibleAnywhere)
	USoundBase* moveSound;
	UPROPERTY(VisibleAnywhere)
	USoundBase* closeSound;
	UPROPERTY(EditAnywhere)
	USoundAttenuation* attenuation;
	FVector target;

	bool isMoving = false;
	bool doOnce = false;
	bool soundDoOnce = false;

	FTimerHandle timer;
	FTimerHandle closeTimer;
	FTimerHandle DEBUG;
};
