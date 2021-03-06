// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Battery.h"
#include "EmptyZoneFinder.h"
#include "CheckPoint.h"
#include "Door.h"
#include "PlasmaDoor.h"
#include "../Spawner/WaveSystem.h"
#include "Blueprint/UserWidget.h"
#include "HackingPanel.h"
#include "Phase1_1.generated.h"

UCLASS()
class HACKED_API APhase1_1 : public AActor
{
	GENERATED_BODY()
	
public:	
	APhase1_1();

	UPROPERTY(EditAnywhere)
	AWaveSystem* waveSystem;
	UPROPERTY(EditAnywhere)
	TArray<ADoor*> doors;
	UPROPERTY(EditAnywhere)
	TArray<AEmptyZoneFinder*> emptyZoneFinders;
	UPROPERTY(EditAnywhere)
	TArray<APlasmaDoor*> plasmaDoors;
	UPROPERTY(EditAnywhere)
	TArray<ABattery*> batteries;
	UPROPERTY(EditAnywhere)
	TArray<ACheckPoint*> checkPoints;
	UPROPERTY(EditAnywhere)
	TArray<AHackingPanel*> hackingPanels;

	//if this true We can Play Phase with only Crusher
	UPROPERTY(EditAnywhere)
	bool DEBUG;

	UFUNCTION(BlueprintPure, Category = "HackingRate")
	float GetCurrentHackingRate();

	UPROPERTY()
	UUserWidget* WG_Progress = nullptr;

	UPROPERTY(EditAnywhere)
	float firstTargetHackingTime;
	UPROPERTY(EditAnywhere)
	float secondTargetHackingTime;
	UPROPERTY(EditAnywhere)
	float thirdTargetHackingTime;

	float firstHackingRate;
	float secondHackingRate;
	float thirdHackingRate;
	
	bool bIsPhase1Running = false;
	bool bIsPhase2Running = false;
	bool bIsPhase3Running = false;

	bool bPhase1Clear = false;
	bool bPhase2Clear = false;
	bool bPhase3Clear = false;

	bool bJumpToPhase2 = false;
	bool bJumpToPhase3 = false;

	bool bIsPhase1Done = false;

	bool bIsPhase2PreActivated = false;
	bool bIsPhase3PreActivated = false;
	
	bool bFirstDoorDoOnce = false;

	uint32 phaseNumber = 1;

	FTimerHandle spawnWaitTimer;

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	//Phase Tool Function
	void LabWaveTimers(float DeltaTime);
	void Phase1Flow();
	void Phase2Flow();
	void Phase3Flow();

	//Phase 1
	void PreparePhase1();
	void StartPhase1();
	void RunningPhase1();
	void EndPhase1();
	
	//Phase 2
	void Phase2PreEnemyStart();
	void PreparePhase2();
	void StartPhase2();
	void RunningPhase2();
	void EndPhase2();

	//Phase 3
	void Phase3PreEnemyStart();
	void PreparePhase3();
	void StartPhase3();
	void RunningPhase3();
	void EndPhase3();

	//END
	void Lab1End();

private:
	// Hacking UI
	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Hacking_Class;
	UPROPERTY()
	class UUserWidget* wgHacking = nullptr;
	float _lastBackingRate = 0.0f;
};
