// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "InGame/AI/BehaviorTree/OldBehaviorTree/BTTask_SRLS_Attack.h"
#include "InGame/Interface/Networkable.h"
#include "SecurityRobot_LS.generated.h"

DECLARE_DELEGATE_OneParam(FOnAttackEndDelegate, UBehaviorTreeComponent*)

UCLASS()
class HACKED_API ASecurityRobot_LS : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASecurityRobot_LS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Reconnect() override;
	virtual void Reconnected() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

public:
	// Do attack!
	void LS_Attack(UBehaviorTreeComponent* OwnerComp);
	// Do next attack anim!
	void LS_NextAttack();
	FOnAttackEndDelegate OnAttackEnd;
	bool OnAttack();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	// anim0 attack
	RPC_FUNCTION(ASecurityRobot_LS, LS_Attack1)
	void LS_Attack1();

	// anim1 attack
	RPC_FUNCTION(ASecurityRobot_LS, LS_Attack2)
	void LS_Attack2();

	// LS AttackDamaging
	void LS_Damaging();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 currentAnimation;

	UPROPERTY()
	class USR_LS_AnimInstance* LS_Anim;
	UBehaviorTreeComponent* savedCp;

	// AI의 변수를 초기화 시킵니다.
	void _ResetAI();

public:
	virtual void OnEnableAI(bool isEnable) override;
	virtual void OnSpawn() override;
};
