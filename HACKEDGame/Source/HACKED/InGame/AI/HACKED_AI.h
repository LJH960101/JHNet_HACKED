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
	// �׾��� �� �ൿ�� ó���ϴ� �Լ�
	virtual void DieProcess();

public:
	// �����ʿ��� ���� ������ ȣ��Ǵ� �Լ�.
	// �ʱ�ȭ�� ���⼭ �ϱ� �ٶ��ϴ�.
	virtual void OnSpawn();

	//virtual void Possessed(AController* NewController) override;

	// ��Ƽ ������ ����Ǿ����� ȣ��Ǵ� �Լ�.
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

	// �÷��̾���� �޴� �Լ�
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

	// ������ ĳ���͸� �����̴� �Լ��Դϴ�.
	// @Param1 velocity : �̵��� �ӵ�
	// @Param2 time : ������ �̵��� �ð�
	// @Param3 slowStartTime = 0.0f : �������� �������� �����ϴ� �ð�. N�� �������� ���� �۵��Ѵ�.
	void HACKEDLaunchCharacter(FVector velocity, float time, float slowStartTime = -1.0f);

private:
	// HACKEDLaunchCharacter���� ����ϴ� ������
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

	// AI�� On Off�ɶ� ����Ǵ� �Լ�.
	virtual void OnEnableAI(bool isEnable) override;

	// �ǰ� ���� ����--------------------------------
	virtual void BeHitAnimation();

	virtual void OnEndHitAnimation();

	RPC_FUNCTION(AHACKED_AI, RPCBeHitAnimation)
		void RPCBeHitAnimation();

	UPROPERTY(VisibleInstanceOnly, Category = BeHit)
		bool bCanBeHitAnimation = true;

	// ���� ���� ����--------------------------------

	virtual void RigidityAI();

	// �ٸ� �÷��̾������ ������ �뺸��.
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

	// ��� �������� ������ �뺸�ϱ� ���� �Լ�
	RPC_FUNCTION(AHACKED_AI, RPCDie)
		void RPCDie();

	// �������� �ٸ� �����鿡�� AI ������ �뺸�ϴ� �Լ�
	RPC_FUNCTION(AHACKED_AI, RPCDestroyAI)
		void RPCDestroyAI();

public:
	// �׾����� �ƴ� �Լ��Դϴ�.
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
	// ������ �ǰ� ó���� �Ͼ�� �Լ�
	// Amount�� ������ Ÿ�Կ� ���� ��� ������ �״� ���� ó���� ��.
	float _TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// �ǰ��� �뺸�ϴ� �Լ�.
	// slot : Ÿ���� ĳ���� ����, damage : ���ط�
	RPC_FUNCTION(AHACKED_AI, RPCTakeDamageToOtherClient, int, float)
	UFUNCTION()
	void RPCTakeDamageToOtherClient(int mySlot, float damage);

	RPC_FUNCTION(AHACKED_AI, RPCAddAgro, bool, int)
	UFUNCTION()
	void RPCAddAgro(bool isCrusher, int amount);
};
