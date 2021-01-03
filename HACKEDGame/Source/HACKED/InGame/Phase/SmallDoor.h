// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/Phase/Door.h"
#include "SmallDoor.generated.h"

UCLASS()
class HACKED_API ASmallDoor : public ADoor
{
	GENERATED_BODY()
public:
	ASmallDoor();
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* doorFrame;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* doorLeft;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* doorRight;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void Open();
	void Close();

protected:
	void LetOpen();
	void LetClose();
};
