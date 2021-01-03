// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "PhaseShelter.generated.h"

UCLASS()
class HACKED_API APhaseShelter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhaseShelter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	UFUNCTION()
		void ShelterAIRestart();
private:
	UPROPERTY(VisibleInstanceOnly, Category = Tutorial_EnemyCount, Meta = (AllowPrivateAccess = true))
		uint32 ShelterEnemyCount = 0;

	bool ShelterEnemyAllDie = false;

	bool bRestartOnce = true;
	UPROPERTY(EditAnywhere)
	class AElevatorDoor* elevatorDoor;
	UPROPERTY(EditAnywhere)
	class ACheckPoint* doorOpenCP;
	UPROPERTY(EditAnywhere)
	class AEmptyZoneFinder* shelterAICheck;
	UPROPERTY(EditAnywhere)
	UAudioComponent* subTitleVoiceComp;
	UPROPERTY(EditAnywhere)
	USoundBase* st1;
	UPROPERTY(EditAnywhere)
	USoundBase* st2;
	UPROPERTY(EditAnywhere)
	class ACheckPoint* exitShelterCP;

	class AHACKEDInGameManager* HACKEDInGameManager;

public:
	bool doorDoOnce = false;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool goodToGo = false;
	bool exitDoOnce = false;
	bool outToPhase1DoOnce = false;
};
