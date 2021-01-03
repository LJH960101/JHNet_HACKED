// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "GameFramework/Character.h"
#include "InGame/AI/BehaviorTree/MNAI_BT/BTTask_MNAI_Attack.h"
#include "MilitaryNamedAI.generated.h"

DECLARE_DELEGATE_OneParam(FOnRagingBlowEndDelegate, UBehaviorTreeComponent*)

UCLASS()
class HACKED_API AMilitaryNamedAI : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMilitaryNamedAI();

	// AI�� On Off�ɶ� ����Ǵ� �Լ�.
	virtual void OnEnableAI(bool isEnable) override;

	// �׾��� �� �ൿ�� ó���ϴ� �Լ�
	virtual void DieProcess() override;

	// AI �ǰ��� ó�� �ϴ� �Լ�
	virtual void BeHitAnimation() override;
	virtual void OnEndHitAnimation() override;

	// AI ������ ó�� �ϴ� �Լ�
	virtual void RigidityAI() override;
	virtual void RigidityEnd() override;


	class UMN_AI_AnimInstance* MN_AI_Anim;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	RPC_FUNCTION(AMilitaryNamedAI,PhotonShot)
	void PhotonShot();

	void RagingBlow(UBehaviorTreeComponent* OwnerComp);

public:
	UFUNCTION()
	void RagingBlowNextRide();

	FOnRagingBlowEndDelegate OnRagingBlowEnded;

	UFUNCTION()
	bool OnRagingBlow();
	
	UFUNCTION()
	void OnRagingBlowMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	UPROPERTY()
		TSubclassOf<class APhotonShot> PhotonCanon;
	UFUNCTION()
	void PhotonShotSpawn();

	UFUNCTION()
	void PhotonShotEnd();
	
	UFUNCTION()
	void RagingBlowDamaging();

private:
	RPC_FUNCTION(AMilitaryNamedAI, RagingBlowFirstRide);
	void RagingBlowFirstRide();

	RPC_FUNCTION(AMilitaryNamedAI, RagingBlowSecondRide);
	void RagingBlowSecondRide();

	UBehaviorTreeComponent* savedCp;

private:
	void StateReset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float RushRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float SwapStateRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float PhotonShotRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
		float RagingBlowRange;

	bool bOnEndPhotonShot = false;

private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bIsPhotonShot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bIsRagingBlow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		float _photonShotExplodeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		float _photonShotExplodeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		float _photonShotSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		float _ragingBlowkDamage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		int32 currentMontage;


};
