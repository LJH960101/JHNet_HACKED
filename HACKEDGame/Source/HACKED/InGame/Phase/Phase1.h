// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "DynamicPlayerBlocker.h"
#include "Phase1.generated.h"

UCLASS()
class HACKED_API APhase1 : public AActor
{
	GENERATED_BODY()
	
public:
	APhase1();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:

	//Master Props
	UPROPERTY(EditAnywhere)
		class AWaveSystem* waveSystem;
	UPROPERTY(EditAnywhere)
		class AEmptyZoneFinder* aiCheckZone;
	UPROPERTY(EditAnywhere)
		class AInteractor* interactor;
	UPROPERTY(EditAnywhere)
	class APhase2* phase2;
	UPROPERTY(VisibleAnywhere)
	bool doOnce=false;

	UPROPERTY(VisibleAnywhere)
	int32 currentSectorNumber=0;

	class AHACKEDInGameManager* HACKEDInGameManager;

	UPROPERTY(VisibleAnywhere)
	bool bIsPhaseRunning = false;
	UPROPERTY(VisibleAnywhere)
	bool bIsPhaseComplete = false;
	//Sector 1 Props
	UPROPERTY(EditAnywhere)
	class ACheckPoint* Sector1CP;
	UPROPERTY(EditAnywhere)
	class ACheckPoint* ShelterOutCP;
	bool shelterOutDoOnce = false;
	//Sector 2 Props
	UPROPERTY(EditAnywhere)
	class AHackingPanel* hackingPanel;
	UPROPERTY(EditAnywhere)
	float hackingTime=15.0f;

	UPROPERTY(EditAnywhere)
		class APathIndicator* sector1PathIndicator;
	UPROPERTY(EditAnywhere)
		class APathIndicator* phase1sector2PathIndicator;
	UPROPERTY(EditAnywhere)
	TArray<ADynamicPlayerBlocker*> blockers;

	FTimerHandle Sector2Timer;
	
	//Sector 3 Props
	UPROPERTY(EditAnywhere)
	float sector3SpawnTime=20.0f;

	FTimerHandle sector3Timer;
	
	//SOUND
	UPROPERTY(EditAnywhere)
	UAudioComponent* audioComp;
	UPROPERTY(EditAnywhere)
	USoundBase* fightBGM;

	UPROPERTY(EditAnywhere)
	UAudioComponent* subTitleVoiceComp;

	UPROPERTY(EditAnywhere)
	USoundBase* st3;

	UPROPERTY(EditAnywhere)
	USoundBase* st4;


	UPROPERTY(EditAnywhere)
	USoundBase* energyCoreAcheive;

	UPROPERTY(EditAnywhere)
	class ACoreBattery* DUMMY;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* hackingProgressWG;

private:
	//PHASE MASTERS
	void StartPhase1();
	void RunningPhase1();
	void EndPhase1();

	//SECTOR 1
	void StartSector1();
	void RunningSector1();
	void EndSector1();

	//SECTOR 2
	void StartSector2();
	void RunningSector2();
	void EndSector2();
	
	//SECTOR 3
	void StartSector3();
	void RunningSector3();
	void EndSector3();

	//VOICE
	FTimerHandle voiceTiemr;
	
	UFUNCTION()
	void Voice_FindCoreBattery();

	UFUNCTION()
	void Voice_AchieveEnergyCore();

	UPROPERTY(EditAnywhere)
	float voiceTimerDelayInterval = 8.0f;
public:
	UFUNCTION(BlueprintCallable)
	float GetHackingPercent() const;

	FORCEINLINE bool GetPhase1CompleteState() { return bIsPhaseComplete; }
};
