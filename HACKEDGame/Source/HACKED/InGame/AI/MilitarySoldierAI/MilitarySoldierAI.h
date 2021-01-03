// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "InGame/AI/BehaviorTree/MSAI_BT/BTTask_MSAI_Attack.h"
#include "MilitarySoldierAI.generated.h"

DECLARE_DELEGATE_OneParam(FOnBladeShockEndDelegate, UBehaviorTreeComponent*)

UCLASS()
class HACKED_API AMilitarySoldierAI : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMilitarySoldierAI();

	// AI가 On Off될때 실행되는 함수.
	virtual void OnEnableAI(bool isEnable) override;

	// 죽었을 때 행동을 처리하는 함수
	virtual void DieProcess() override;

	// AI 피격을 처리 하는 함수
	virtual void BeHitAnimation() override;
	virtual void OnEndHitAnimation() override;

	// AI 경직을 처리 하는 함수
	virtual void RigidityAI() override;
	virtual void RigidityEnd() override;

	// AI State 초기화
	void StateReset();

	class UMS_AI_AnimInstance* MS_AI_Anim;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float LaserAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float SwapAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float BladeAttackRange;

	bool bOnEndLaserShock = false;

public:
	UPROPERTY(VisibleInstanceOnly)
		int32 TutorialBarrierHitCount;

public:
	RPC_FUNCTION(AMilitarySoldierAI, LaserShock);
	void LaserShock();

	void BladeShock(UBehaviorTreeComponent* OwnerComp);

private:
	UFUNCTION()
	void GetPlayerPos();

	FVector FirstTargetingPos;

	UFUNCTION()
	void LaserShockDamaging();

	UFUNCTION()
	void BladeShockDamaging();

public:
	UFUNCTION()
	void BladeNextAttack();

	FOnBladeShockEndDelegate OnBladeShockEnded;

	UFUNCTION()
	bool OnBladeAttack();

	UFUNCTION()
	void OnBladeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	RPC_FUNCTION(AMilitarySoldierAI, BladeFirstAttack);
	void BladeFirstAttack();

	RPC_FUNCTION(AMilitarySoldierAI, BladeSecondAttack);
	void BladeSecondAttack();


	UBehaviorTreeComponent* savedCp;

public:

	UFUNCTION()
	void LaserShockEnd();

private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bIsLaserShock;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bIsBladeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		float _laserAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		float _bladeAttackDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		int32 currentMontage;

private:
	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* PC_LaserShock = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PS_LaserShock;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PS_LaserShockHit;

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* PC_MoveBurst = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PS_MoveBurst;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PS_BladeShockHit;





};
