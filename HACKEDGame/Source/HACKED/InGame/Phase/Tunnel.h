// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Tunnel.generated.h"

UCLASS()
class HACKED_API ATunnel : public AActor
{
	GENERATED_BODY()
	
public:	
	ATunnel();

	void OutlineOn();
	void OutlineOff();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* tunnel;
};
