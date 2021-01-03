// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "SecurityRobot_ET.generated.h"

UCLASS()
class HACKED_API ASecurityRobot_ET : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASecurityRobot_ET();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 죽었을 때 행동을 처리하는 함수
	virtual void DieProcess() override;

public:	
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 공격의 끝을 알리는 변수.
	// BT_Attack에서 사용합니다.
	bool bOnEndAttack = false;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsElectricShock;

	void _ResetAI();

public:
	class USR_ET_AnimInstance* SR_ET_Anim;


public:
	void ET_Rigidity();

	RPC_FUNCTION(ASecurityRobot_ET, ElectricShock);
	void ElectricShock();
	void ElectricShockDamaging();
	
	UFUNCTION()
	void OnAttackEnd();

public:
	virtual void OnSpawn() override;
};
