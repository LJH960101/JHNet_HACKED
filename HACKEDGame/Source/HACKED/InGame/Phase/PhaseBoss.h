// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "PhaseBoss.generated.h"

UCLASS()
class HACKED_API APhaseBoss : public AActor
{
	GENERATED_BODY()
	
public:	
	APhaseBoss();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void StartBossPhase();
	UFUNCTION()
	void RunningBossPhase();

	UFUNCTION()
		void PlayLoopSound();

	UPROPERTY(EditAnywhere)
		class USoundWave* GoToDoorAgain;
private:
	class AHACKEDInGameManager* HACKEDInGameManager;

	UPROPERTY(EditAnywhere)
	class AInGameNetworkProcessor* InGameNetwork;
	UPROPERTY(EditAnywhere)
	class AMilitaryBossAI* boss;
	UPROPERTY(EditAnywhere)
		class AEmptyZoneFinder* aiCheckZone;
	UPROPERTY(EditAnywhere)
		float toCreditMaxTime = 3.0f;
	UPROPERTY(EditAnywhere)
		float toLobbyMaxTime = 25.0f;
	UPROPERTY(EditAnywhere)
		float soundFadeInTime = 5.0f;
	UPROPERTY(EditAnywhere)
		float soundFadeOutTime = 5.0f;
	UPROPERTY(EditAnywhere)
		class UAudioComponent* bossSoundComp;
	UPROPERTY(EditAnywhere)
		class USoundWave* bossSoundIntro;
	UPROPERTY(EditAnywhere)
		class USoundWave* bossSoundLoop;

	float toLobbyTimer = 0.0f;
	bool bBossAlive = false;
	bool doOnce = false;
	FTimerHandle endTimer;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BossPhase)
	bool bIsBossPhaseRunning = false;
};
