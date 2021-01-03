// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/HACKEDInGameManager.h"
#include "GameFramework/Actor.h"
#include "Phase3.generated.h"

UCLASS()
class HACKED_API APhase3 : public AActor
{
	GENERATED_BODY()

public:
	APhase3();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE void SetPhaseRunningState(bool _running) { bIsPhaseRunning = _running; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
		class UNetworkBaseCP* NetBaseCP;
private:

	//Master Props
	UPROPERTY(EditAnywhere)
		class APhaseBoss* bossPhase;
	UPROPERTY(EditAnywhere)
		class AWaveSystem* waveSystem;
	UPROPERTY(EditAnywhere)
		class AInGameNetworkProcessor* InGameNetWork;

	UPROPERTY(EditAnywhere)
		class AEmptyZoneFinder* aiCheckZone;
		bool doOnce = false;
		bool endDoOnce = false;
		bool explodeDoOnce = false;
		bool enterDoOnce = false;
		bool gameOverOnce = false;

	UPROPERTY(EditAnywhere)
		class APathIndicator* phase2to3PathIndicator;
	UPROPERTY(VisibleAnywhere)
		bool bIsPhaseRunning = false;
	UPROPERTY(EditAnywhere)
		class ACheckPoint* phase3StartCP;
	UPROPERTY(EditAnywhere)
		class ACheckPoint* phase3EndCP;
	UPROPERTY(EditAnywhere)
		class AExplosiveObject* explosiveObject;
	UPROPERTY(EditAnywhere)
	class AActor* fence;
	UPROPERTY(EditAnywhere)
	class ADynamicPlayerBlocker* blocker;
	UPROPERTY(EditAnywhere)
	class ACheckPoint* phase3EnterCP;

	class AHACKEDInGameManager* HACKEDInGameManager;

	UPROPERTY(EditAnywhere)
	UAudioComponent* subTitleVoiceComp;

	UPROPERTY(EditAnywhere)
	USoundBase* st7;

	UPROPERTY(EditAnywhere)
	USoundBase* st8;

	UPROPERTY(EditAnywhere)
	USoundBase* st9;

	UPROPERTY(EditAnywhere)
	USoundBase* GoBossSound;

	UPROPERTY(EditAnywhere)
	USoundBase* BossIntroSound;

	UPROPERTY(EditAnywhere)
	USoundBase* EndingCreditSound;

public:
	UPROPERTY(EditAnywhere, Category = LinkPos)
		TArray<AActor*> BossPhaseLinkPos;

public:
	//PHASE MASTERS
	RPC_FUNCTION(APhase3, RPCGoToBossCheat)
	void RPCGoToBossCheat();


	UFUNCTION()
	void StartPhase3();

	RPC_FUNCTION(APhase3, BossProductionStart)
	UFUNCTION()
	void BossProductionStart();

	RPC_FUNCTION(APhase3, BossProduction)
		UFUNCTION()
		void BossProduction();

private:
	UFUNCTION()
	void RunningPhase3();
	
	UFUNCTION()
	void EndPhase3();
	
	UFUNCTION()
	void BossProductionEnd();

	UFUNCTION()
	void ReachDoorBackToLobby();
	

	FTimerHandle BossProductionTimer;

	FTimerHandle EndPhase3Timer;

	FTimerHandle questTimer;
	void StartQuest();
};
