// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Phase2.generated.h"

UCLASS()
class HACKED_API APhase2 : public AActor
{
	GENERATED_BODY()

public:
	APhase2();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE void SetPhaseRunningState(bool _running) { bIsPhaseRunning = _running; }
	void StartPhase2();

private:
	//Master Props
	UPROPERTY(EditAnywhere)
	class AWaveSystem* waveSystem;
	UPROPERTY(EditAnywhere)
		class AEmptyZoneFinder* aiCheckZone;
	UPROPERTY(EditAnywhere)
		class APhase1* phase1;
	class AHACKEDInGameManager* HACKEDInGameManager;
	UPROPERTY(EditAnywhere)
		UAudioComponent* audioComp;
	UPROPERTY(EditAnywhere)
		USoundBase* fightBGM;

	UPROPERTY(EditAnywhere)
	UAudioComponent* subTitleVoiceComp;

	UPROPERTY(EditAnywhere)
	USoundBase* st5;
	UPROPERTY(EditAnywhere)
	USoundBase* st6;

	UPROPERTY(EditAnywhere)
		class APathIndicator* phase1sector2PathIndicator;
	UPROPERTY(VisibleAnywhere)
		bool doOnce = false;
	UPROPERTY(VisibleAnywhere)
		bool doOncePhase1 = false;
	UPROPERTY(VisibleAnywhere)
		bool doOncePhase2 = false;
	UPROPERTY(VisibleAnywhere)
		bool bIsPhaseRunning = false;

	UPROPERTY(EditAnywhere)
		class ACheckPoint* phase1EndCP;
	UPROPERTY(EditAnywhere)
		class ACheckPoint* phase2StartCP;
	UPROPERTY(EditAnywhere)
	class ATunnel* tunnel;
	UPROPERTY(EditAnywhere)
		class APhase3* phase3;

	UPROPERTY(EditAnywhere)
		uint32 enemyCount = 0;

	UPROPERTY(VisibleAnywhere)
	bool bIsTunnelEntered=false;
	UPROPERTY(VisibleAnywhere)
	bool bIsDelayFinished=false;

	FTimerHandle delayTimer;
	FTimerHandle questTimer;
private:
	//PHASE MASTERS
	
	void StartQuest();
	void RunningPhase2();
	void EndPhase2();
	void DelayEnemyCount();
};
