// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Pawn.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "InGame/Network/Component/NetworkTransformCP.h"
#include "InGame/Network/ReplicatableVar.h"
#include "Tuple.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "AIStatComponent.h"
#include "InGame/Interface/HACKED_Enemy.h"
#include "InGame/HACKEDTool.h"
#include "InGame/Interface/Networkable.h"
#include "InGame/HACKEDInGameManager.h"
#include "HACKED_AI.generated.h"

#define ENABLE_TRANSFORM_SYNC

const float CRITICAL_DAMAGE_LIMIT = 100.0f;

UCLASS()
class HACKED_API AHACKED_AI : public ACharacter, public IHACKED_Enemy, public INetworkable
{
	GENERATED_BODY()

protected:
	// 죽었을 때 행동을 처리하는 함수
	virtual void DieProcess();

public:
	// 스포너에서 스폰 됬을때 호출되는 함수.
	// 초기화를 여기서 하길 바랍니다.
	virtual void OnSpawn();

	//virtual void Possessed(AController* NewController) override;

	// 파티 슬롯이 변경되었을때 호출되는 함수.
	virtual void OnSlotChange(bool isMaster);

public:
	UPROPERTY(EditAnywhere, Category = EsperStack)
		float firstStackDamage = 25.0f;

	UPROPERTY(EditAnywhere, Category = EsperStack)
		float secondStackDamage = 60.0f;

	UPROPERTY(EditAnywhere, Category = EsperStack)
		float thirdStackDamage = 100.0f;

	UPROPERTY(EditAnywhere, Category = EsperStack)
		float criticalDamage = 100.0f;

	UPROPERTY(VisibleAnywhere, Category = EsperStack)
		float currentStack;

	UPROPERTY(VisibleAnywhere, Category = DamageOutput)
		int damageAmountOutput;

	float maxStack = 4.0f;

protected:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Hp,
		Meta = (AllowPrivateAccess = true))
		bool bCanBossRigidity = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Hp,
		Meta = (AllowPrivateAccess = true))
		bool bCanBossBeHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Detect)
		float EsperAgroGauge = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Detect)
		float CrusherAgroGauge = 500.0f;


private:
	FHACKED_AIStat aiStat;
	TArray<AHACKEDCharacter*> _players;

	// 플레이어들을 받는 함수
	void RefreshPlayers();

	class ASpawner* _spanwer;
	FTransform _meshOriginTransform;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = AI)
		class UWidgetComponent* HackedAIWidget;

public:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PS_BarrierHit;


public:
	// Sets default values for this pawn's properties
	AHACKED_AI();

	virtual void Tick(float DeltaSeconds) override;

public:
	FTimerHandle DestroyTimeHandle;
	float destroyTime;

	// 강제로 캐릭터를 움직이는 함수입니다.
	// @Param1 velocity : 이동할 속도
	// @Param2 time : 강제로 이동할 시간
	// @Param3 slowStartTime = 0.0f : 선형으로 느려지기 시작하는 시간. N초 남았을때 부터 작동한다.
	void HACKEDLaunchCharacter(FVector velocity, float time, float slowStartTime = -1.0f);

private:
	// HACKEDLaunchCharacter에서 사용하는 변수들
	FVector _launchVelocity;
	bool _onLaunch = false;
	float _launchTime = 0.0f;
	float _launchSlowStartTime = -1.0f;
	float _launchSlowTimer = -1.0f;



public:
	UPROPERTY(VisibleInstanceOnly, Category = AI_Intelligence)
		bool bTakenDamage = false;

public:
	UFUNCTION(BlueprintPure, Category = Hp)
		float GetAIHP();

	UFUNCTION(BlueprintPure, Category = Hp)
		float GetAIStack();

public:
	// Init AI Stat
	void InitAIStatByType(const EHackedAIType& type);
	// AI Max Hp
	float GetMaxHp();
	// AI Decorator Attack Range
	float GetAttackRange();
	// AI Detecting Range 
	float GetDetectingRange();
	// AI MoveSpeed
	float GetAIMoveSpeed();
	// AI AttackDamage
	float GetAIAttackDamage();
	// AI Patrol Range
	float GetAIPatrolRange();

	// Get Nearest player's address and distance
	TTuple<AActor*, float> GetNearestPlayer();
	TArray<AHACKEDCharacter*> GetPlayers();

	FVector NearestPlayerPos;

	// AI가 On Off될때 실행되는 함수.
	virtual void OnEnableAI(bool isEnable) override;

	// 피격 관련 로직--------------------------------
	virtual void BeHitAnimation();

	virtual void OnEndHitAnimation();

	RPC_FUNCTION(AHACKED_AI, RPCBeHitAnimation)
		void RPCBeHitAnimation();

	UPROPERTY(VisibleInstanceOnly, Category = BeHit)
		bool bCanBeHitAnimation = true;

	// 경직 관련 로직--------------------------------

	virtual void RigidityAI();

	// 다른 플레이어들한테 경직을 통보함.
	RPC_FUNCTION(AHACKED_AI, RPCRigidityAI)
		void RPCRigidityAI();

	virtual void RigidityEnd();

	UPROPERTY(VisibleInstanceOnly, Category = Rigidity)
		bool bCanRigidity = true;
private:
	UFUNCTION()
		void RigidityCoolDown();

	FTimerHandle RigidityHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rigidity, Meta = (AllowPrivateAccess = true))
		float _rigityCoolTime;


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
		class UNetworkBaseCP* NetBaseCP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
		class UNetworkTransformCP* NetTransformCP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class AHACKEDInGameManager* HACKEDInGameManager;

private:
	bool _bDie;
	CReplicatableVar<bool> _bOnAI;

protected:

	bool _bIsBossDie = false;

	void DieSpawnEmitter();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PS_DieExplosion;

	// 모든 유저에게 죽음을 통보하기 위한 함수
	RPC_FUNCTION(AHACKED_AI, RPCDie)
		void RPCDie();

	// 서버에서 다른 유저들에게 AI 삭제를 통보하는 함수
	RPC_FUNCTION(AHACKED_AI, RPCDestroyAI)
		void RPCDestroyAI();

public:
	// 죽었는지 아는 함수입니다.
	UFUNCTION(BlueprintPure)
		bool IsDie();

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
		bool bPatrolCheck;

	UPROPERTY(EditAnywhere, Category = BehaviorTree)
		bool bIsAIImmortal = false;

	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UHpComponent* HpComponent;

	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UAIStatComponent* AI_Stat;

	class AHACKEDCharacter* Hacked_Character;


	virtual void PostInitializeComponents() override;
	void SetDie(bool isDie);

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	// 실제로 피격 처리가 일어나는 함수
	// Amount를 데미지 타입에 따라 깎고 스택을 쌓는 등의 처리가 들어감.
	float _TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// 피격을 통보하는 함수.
	// slot : 타격한 캐릭터 슬롯, damage : 피해량
	RPC_FUNCTION(AHACKED_AI, RPCTakeDamageToOtherClient, int, float)
	UFUNCTION()
	void RPCTakeDamageToOtherClient(int mySlot, float damage);

	RPC_FUNCTION(AHACKED_AI, RPCAddAgro, bool, int)
	UFUNCTION()
	void RPCAddAgro(bool isCrusher, int amount);
};
