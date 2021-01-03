// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/Phase/Door.h"
#include "BigDoor.generated.h"

UCLASS()
class HACKED_API ABigDoor : public ADoor
{
	GENERATED_BODY()

public:
	ABigDoor();
	UPROPERTY(EditAnywhere)
	USceneComponent* doorFrame;
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

private:
	void LetOpen();
	void LetClose();
};
