// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/BehaviorTree/MBAI_BT/BTTask_MBAI_Attack.h"
#include "InGame/Interface/Networkable.h"
#include "InGame/Network/ReplicatableVar.h"
#include "MilitaryBossAI.generated.h"

// Boss Difficulty

UENUM(BlueprintType)
enum class EBossDifficulty : uint8
{
	EASY	UMETA(DisplayName = "EASY"),
	NORMAL  UMETA(DisplayName = "NORMAL"),
	HARD	UMETA(DisplayName = "HARD")
};

DECLARE_DELEGATE_OneParam(FOnHammerShotEndDelegate, UBehaviorTreeComponent*)

UCLASS()
class HACKED_API AMilitaryBossAI : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMilitaryBossAI();

	UPROPERTY(EditAnywhere)
	AActor* TeleportLocation;

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

	class UMB_AI_AnimInstance* MB_AI_Anim;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	RPC_FUNCTION(AMilitaryBossAI, MagneticPulseOn)
	void MagneticPulseOn();

public:

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_Hammering;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_SlapRushing;

private:
	void StateReset();

	RPC_FUNCTION(AMilitaryBossAI, BossDieExplosion)
	UFUNCTION()
	void BossDieExplosion();

	UPROPERTY(VisibleAnywhere)
		USoundWave* BossExplosionSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PostInitializeComponents() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//---------Difficulty System---------//
public:
	RPC_FUNCTION(AMilitaryBossAI, RPCSetBossDifficulty, int)
	void RPCSetBossDifficulty(int bossDifficulty); // 1~3

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = BossDifficulty)
	bool bIsEasyMode = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = BossDifficulty)
	bool bIsNormalMode= false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = BossDifficulty)
	bool bIsHardMode= false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = BossDifficulty)
	float patternDistance = 1000.0f;

private:
	UFUNCTION()
	void SetPatternFrequency(const EBossDifficulty& bossDifficulty);

	//---------Agro System---------//
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = AgroSystem)
	bool changeToNearTarget = false;

private:
	RPC_FUNCTION(AMilitaryBossAI, _SetChangeToNearTarget, bool)
	void _SetChangeToNearTarget(bool isOn);

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = AgroSystem)
	int crusherAgroCount = 0;

	bool ForcedNearestTarget(int percent);

	// 현재 타겟 플레이어를 가져온다.
	AHACKEDCharacter* GetAgroPlayer();

private:

	// 어그로 타겟을 갱신한다.
	UFUNCTION()
	void RefreshAgroPlayer();

	// 현재 타겟 동기화용 변수
	CReplicatableVar<bool> _bAgroIsCrusher;

	UFUNCTION()
	void DieAgroReset();

	UPROPERTY(VisibleInstanceOnly, Category = AgroSystem)
		bool bChangeTargetDelay = true;

	UPROPERTY(VisibleInstanceOnly, Category = AgroSystem)
		bool bDieAgroReset = false;

	FTimerHandle ChangeToTargetHandle;


	//---------Military Boss Hammer Shot State---------//
public:

	FOnHammerShotEndDelegate OnHammerShotEnd;

	UFUNCTION()
	void HammerShot(UBehaviorTreeComponent* OwnerComp);

	UBehaviorTreeComponent* savedCp;

	RPC_FUNCTION(AMilitaryBossAI, RPCHammerShot)
	UFUNCTION()
	void RPCHammerShot();

	UFUNCTION()
	float GetHammerShotRange();

private:

	UFUNCTION()
	void HammerShotDamaging();

	UFUNCTION()
	void HammerShotEnd();


	//---------Military Boss Machine Gun State---------//
public:

	RPC_FUNCTION(AMilitaryBossAI, MachineGun)
	UFUNCTION()
	void MachineGun();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UParticleSystemComponent* PC_BulletMuzzleLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UParticleSystemComponent* PC_BulletMuzzleRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UParticleSystem* PS_BulletMuzzle;

private:

	UFUNCTION()
	void MachineGunShootingStart();

	UFUNCTION()
	void MachineGunDamaging();

	UFUNCTION()
	void MachineGunEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Bullet Projectiles")
		TSubclassOf<class AMachineGunBullet> BulletBP;

	UPROPERTY()
		UParticleSystem* PS_BulletHit;

	FTimerHandle MachineGunHandle;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _machineGunTime = 3.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _bulletFrequency = 0.05f;

	bool BulletSpawnPosCheck = false;

	//---------Homing Missile State---------//
public:

	RPC_FUNCTION(AMilitaryBossAI, HomingMissile)
	UFUNCTION()
	void HomingMissile();

	UFUNCTION()
	void HomingMissileStart();

	UFUNCTION()
	void HomingMissileLaunch();

	UFUNCTION()
	void HomingMissileEnd();

	FTimerHandle HomingMissileHandle;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _missileLaunchTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = Missile)
		TSubclassOf<class AHomingMissile> HomingMissileBP;

	bool MissileSpawnPosCheck = false;

	//---------Slap Rush State---------//
public:

	RPC_FUNCTION(AMilitaryBossAI, SlapRush)
	UFUNCTION()
		void SlapRush();

	UFUNCTION()
		void SlapRushDamaging();

	UFUNCTION()
		void SlapRushing();

	UFUNCTION()
		void SlapRushFinish();

	FVector SlapRushStartPos;
	FVector SlapRushDirection;



	//---------Vortex Beam State---------//

	RPC_FUNCTION(AMilitaryBossAI, VortexBeam)
	UFUNCTION()
		void VortexBeam();


public:
	UFUNCTION()
		void PowerShieldDelay();

	RPC_FUNCTION(AMilitaryBossAI, SetImmortalMode, bool)
		UFUNCTION()
		void SetImmortalMode(bool isOn);

private:

	UFUNCTION()
		void PowerShieldDamage();

	FTimerHandle ShieldDamageHandle;

	UPROPERTY(EditAnywhere, Category = PowerShield, Meta = (AllowPrivateAccess = true))
		float _powerShieldDelay = 0.1f;


public:
	UPROPERTY(EditAnywhere, Category = Immortal)
		class UParticleSystemComponent* PC_PowerShield;

	UPROPERTY(EditAnywhere, Category = Immortal)
		class UParticleSystem* PS_PowerShield;

	UPROPERTY(EditAnywhere, Category = Immortal)
		class UParticleSystem* PS_PowerShieldStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UParticleSystem* PS_BossExplosion;

	UFUNCTION()
		float GetBossHp();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Primary,
		Meta = (AllowPrivateAccess = true))
		float _moveSpeed = 550.0F;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Primary,
		Meta = (AllowPrivateAccess = true))
		float _maxHp = 20000.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Immortal,
		Meta = (AllowPrivateAccess = true))
		bool _bFirstImmortalCheck = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Immortal,
		Meta = (AllowPrivateAccess = true))
		bool _bSecondImmortalCheck = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Immortal,
		Meta = (AllowPrivateAccess = true))
		bool _bThirdImmortalCheck = true;

public:
		UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Immortal)
			bool _bIsBossImmortal = false;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		bool bIsHammering = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = MachineGun,
		Meta = (AllowPrivateAccess = true))
		bool bIsMachineGun = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HomingMissile,
		Meta = (AllowPrivateAccess = true))
		bool bIsHomingMissile = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HomingMissile,
		Meta = (AllowPrivateAccess = true))
		bool bIsSlapRush = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = HomingMissile,
		Meta = (AllowPrivateAccess = true))
		bool bIsVortexBeam = false;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = MachineGun)
		bool bOnEndMachineGun = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = MachineGun)
		bool bOnEndHomingMissile = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = MachineGun)
		bool bOnEndSlapRush = false;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _hammerShotDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _hammerShotRange = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _bulletShotDamage = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HammerShot,
		Meta = (AllowPrivateAccess = true))
		float _missileExpDamage = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlapRush,
		Meta = (AllowPrivateAccess = true))
		float _slapRushPower = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlapRush,
		Meta = (AllowPrivateAccess = true))
		float _slapRushDistance = 3000.0f;




public:
	UFUNCTION(BlueprintCallable, Category = Widget)
	void AddHPWidgetToViewport();

	UFUNCTION(BlueprintCallable, Category = Widget)
	void RemoveHPWidgetToViewport();

	UPROPERTY(EditAnywhere)
		class UAudioComponent* BossSoundComp;

	UPROPERTY(EditAnywhere)
		class USoundWave* ImmortalCautionSound;

};
