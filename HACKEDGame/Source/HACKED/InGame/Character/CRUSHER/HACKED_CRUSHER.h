#pragma once

#include "HACKED.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "CRUSHER_StatComponent.h"
#include "Blueprint/UserWidget.h"
#include "HACKED_CRUSHER.generated.h"

DECLARE_MULTICAST_DELEGATE(FVoidNotifyDelegate);
class AHACKED_AI;

UCLASS()
class HACKED_API AHACKED_CRUSHER : public AHACKEDCharacter
{
	GENERATED_BODY()

public:
	AHACKED_CRUSHER();

	//NATIVE FUNCTIONS
	virtual void Possessed(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	bool IsOnSelfHealing();

private:
	// Aim ������ ���۴ϴ�.
	// ���ӿ������� ����մϴ�.
	void DisableAimWidget();

	/*-------------------------------------------------- ## Basic ## --------------------------------------------------*/

public:
	FHACKED_CRUSHER_Stat crusherStat;
	UPROPERTY()
		TSubclassOf<class UUserWidget> WG_Aim_Class;
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UUserWidget* WG_Aim = nullptr;
	UPROPERTY()
		class UCRUSHER_AnimInstance* animInstance;
	UPROPERTY(EditAnywhere)
		class USphereComponent* checkSphere;
	UPROPERTY(VisibleAnywhere)
		class USphereComponent* rocketRushSphere;
	UPROPERTY(EditAnywhere)
		class UParticleSystemComponent* jetPackEffectComp;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* jetPackEffect;
public:
	UFUNCTION(BlueprintPure, Category = SkillSystem)
		virtual int GetSkillRate(int skill);
	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UCRUSHER_StatComponent* Crusher_Stat;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_CrusherPA;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_RocketRush;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_GroundSmash;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_CrusherUltimateAttack;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_OnAdrenalSurge;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_VitalityShield;

	UPROPERTY(EditAnywhere)
		USoundWave* rr_DashSound;
	UPROPERTY(EditAnywhere)
		USoundWave* rr_BoosterSound;
	UPROPERTY(EditAnywhere)
		USoundAttenuation* rr_Attenuation;

	//FUNCTIONS
	void InitCrusherStat();
	float GetMaxHp();
	void JetPackEffectStart();
	void JetPackEffectStop();
	FORCEINLINE USphereComponent* GetCheckSphere() const { return checkSphere; }

public:
	RPC_FUNCTION(AHACKED_CRUSHER, CrusherIntroSkipStart)
	UFUNCTION()
		void CrusherIntroSkipStart();

	UFUNCTION()
		void CrusherIntroSkip();

	RPC_FUNCTION(AHACKED_CRUSHER, CrusherIntroSkipStop)
	UFUNCTION()
		void CrusherIntroSkipStop();

	UFUNCTION(BlueprintImplementableEvent)
		void CrusherTurnOffSkipUI();

	/*-------------------------------------------------- ## Primary ## --------------------------------------------------*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
		float _crusherMaxHp = 1000.0f;

	/*-------------------------------------------------- ## Attack ## --------------------------------------------------*/

	//Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bIsAttackState = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* primaryAttackEffectComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* primaryAttackEffect;
	//�⺻ ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float normalDamage = 50.0f;
	//ũ��Ƽ�� ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float criticalDamage = 80.0f;
	//�׶��� ���޽� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		float groundSmashRadius = 500.0f;
	//�׶��� ���޽� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		float groundSmashDamage = 150.0f;
	
	//Functions
	RPC_FUNCTION(AHACKED_CRUSHER, Attack)
	void Attack();
	
public:
	UFUNCTION()
		void SpawnPrimaryHitEffect(FVector location);
	FORCEINLINE bool GetAttackState() const { return bIsAttackState; }
	FORCEINLINE void SetAttackState(bool _attackState) { bIsAttackState = _attackState; }

	/*-------------------------------------------------- ## Rocket Rush ## --------------------------------------------------*/
public:
	UFUNCTION()
		void RushDamageChange(float changeValue);

private:
	//Variables
	UPROPERTY()
		TSubclassOf<class UUserWidget> WG_RocketRush_Class;
	UPROPERTY()
		class UUserWidget* WG_RocketRush = nullptr;
	//���� ���� �ִ� �Ÿ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MaxRushDistance = 2000.0f;
	//���� ���� ��� �Ÿ�
	UPROPERTY()
		float rr_RushDistance = 0.0f;
	//���� ���� ���� ��¡ �ð�
	UPROPERTY()
		float rr_Charging = 0.0f;
	//���� ���� �ִ� ��¡ �ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MaxCharging = 2.0f;
	//���� ���� �ּ� ��¡ �ð� (�� �ð��� ������ �� ��ų ��� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MinCharging = 0.4f;
	//���� �о�� ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_PushPower = 3000.0f;
	//������ ���� �����ϴ� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_RushSpeed = 3000.0f;
	//���� ������ ����� �ּ� Velocity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MinVelocity = 700.0f;
	//���� ������ ���� �� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_Damage = 100.0f;
	//�ʹ� Velocity�� MinVelocity�� ���� ���ϴ� ��찡 �־� �߰��ϴ� ���� ���ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_checkWaitTime = 0.0f;
	//�ִ� ���� ���ð�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_checkWaitMaxTime = 0.15f;
	//��¡ ���ΰ�
	UPROPERTY()
		bool bIsRocketRushChargingState = false;
	//���� ���� ���ΰ�
	UPROPERTY()
		bool bIsRocketRushLaunchingState = false;
	//���� ������ �Ÿ� ����� ���� ��ġ ���� ����
	UPROPERTY()
		FVector rr_StartLocation;
	//���� ������ ���� ���� ����
	UPROPERTY()
		FVector rr_ForwardVector;
	//���� ������ ���� ����ü
	UPROPERTY()
		TArray<AHACKED_AI*> rr_Hitted;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* rr_Foot_DirtTrailComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* rr_Foot_DirtTrail;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent* rr_Collision_EffectComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		UParticleSystem* rr_Collision_Effect;

	//Functions
	UFUNCTION()
	void RocketRushOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	RPC_FUNCTION(AHACKED_CRUSHER, RocketRushStart)
	void RocketRushStart();
	void RocketRushCharging();

	RPC_FUNCTION(AHACKED_CRUSHER, RocketRushLaunch)
	void RocketRushLaunch();

	void RocketRushing();
	bool RocketRushingCheck() const;

	RPC_FUNCTION(AHACKED_CRUSHER, RocketRushStop)
	void RocketRushStop();

	UFUNCTION(BlueprintCallable)
		void RocketRushEffectHit(FVector location);
	UFUNCTION(BlueprintCallable)
		void RocketRushEffectStart();
	UFUNCTION(BlueprintCallable)
		void RocketRushEffectStop();

	UFUNCTION(BlueprintCallable)
		void RocketRushSoundPlay();


public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE float GetRocketRushChargingPercent() const { return rr_Charging / rr_MaxCharging; }
	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetRocketRushLaunchingState() const { return bIsRocketRushLaunchingState; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetRocketRushChargingState() const { return bIsRocketRushChargingState; }
	/*-------------------------------------------------- ## Barrier Generator ## --------------------------------------------------*/
private:
	//Functions
	RPC_FUNCTION(AHACKED_CRUSHER, BarrierGenerator)
	void BarrierGenerator();

	/*-------------------------------------------------- ## Punish Shield ## --------------------------------------------------*/
	RPC_FUNCTION(AHACKED_CRUSHER, PunishShield);
	UFUNCTION()
		void PunishShield();

	UFUNCTION()
		void PunishShieldDamage();

	UFUNCTION()
		void PunishShieldStun();

	UFUNCTION()
		void PunishShieldEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PunishShield, Meta = (AllowPrivateAccess = true))
		float _punishShieldRange = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PunishShield, Meta = (AllowPrivateAccess = true))
		float _punishShieldDamage = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PunishShield, Meta = (AllowPrivateAccess = true))
		float _punishStunDelay = 0.5f;

	TArray<class AHACKED_AI*> PunishShieldCheckAI;

	FTimerHandle PunishShieldStunDelayTimer;

public:
	UFUNCTION()
		void PunishDamageChange(float changeValue);


public:
	/*-------------------------------------------------- ## NotYet ## --------------------------------------------------*/

	/*-------------------------------------------------- ## Ultimate ## --------------------------------------------------*/
public:
	RPC_FUNCTION(AHACKED_CRUSHER, AdrenalSurge)
	UFUNCTION()
	void AdrenalSurge();
	UFUNCTION()
		void OnAdrenalSurge();
	UFUNCTION()
		void AdrenalSurgeEnd();
	UFUNCTION(BlueprintPure, Category="Ultimate")
	float GetUltimateRemainGuage();
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Tutorial_Ultimate)
		bool bCanTutoUltimate = false;
	FTimerHandle CrusherUltimateHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate, Meta = (AllowPrivateAccess = true))
		float _adrenalTime = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate, Meta = (AllowPrivateAccess = true))
		class UParticleSystemComponent* ult_Buff_EffectComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* ult_Buff_Effect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		float ultNormalDamage = 120.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		float ultGroundSmashRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		float ultGroundSmashDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		bool bCooperationUltimateCheck = false;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VitalityShield)
		bool bIsVitalityShield = false;
	UFUNCTION()
		void SpawnUltimateDustEffectComp(FVector location);
	void UltimateBuffEffectStart();
	void UltimateBuffEffectStop();
	/*-------------------------------------------------- ## Vitality Shield ## --------------------------------------------------*/
	RPC_FUNCTION(AHACKED_CRUSHER, VitalityShield)
	UFUNCTION()
	void VitalityShield();
	UFUNCTION()
		void VitalityShieldStart();
	UFUNCTION()
		void VitalityHealing();
	UFUNCTION()
		void VitalityShieldEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = VitalityShield)
	void ShowVitalityErrorWidget(bool vType);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
		float linkedSkillRange = 4000.0f;
	float esperShieldAmount;
	float crusherShieldAmount;
	FTimerHandle VitalityShieldHandle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
		float _vitalityTime;

	UPROPERTY(VisibleInstanceOnly)
		bool tutoVitalityUsedCheck = false;
	/*-------------------------------------------------- ## Dash ## --------------------------------------------------*/
	RPC_FUNCTION(AHACKED_CRUSHER, CrusherDash)
	UFUNCTION()
	void CrusherDash();
	UFUNCTION()
		void CrusherDashing();
	RPC_FUNCTION(AHACKED_CRUSHER, CrusherDashFinish)
	UFUNCTION()
		void CrusherDashFinish();
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dash_CrusherDash, Meta = (AllowPrivateAccess = true))
		bool bIsCrusherDash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash_CrusherDash, Meta = (AllowPrivateAccess = true))
		float _crusherDashDistance = 700.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash_CrusherDash, Meta = (AllowPrivateAccess = true))
		float _crusherDashPower;
	FVector DashStartPos;
	FVector DashDirection;
	/*-------------------------------------------------- ## Self Healing ## --------------------------------------------------*/

	RPC_FUNCTION(AHACKED_CRUSHER, RPCSelfHealing)
	UFUNCTION() 	// �ڰ� ġ�� ���¿� �����ϴ� �Լ��Դϴ�.
		void RPCSelfHealing();

	// �ڰ� ġ���� �������� �����ϴ� �Լ��Դϴ�.
	UFUNCTION()
		void SelfHealingEnd();

	// �ڰ� ġ�� �ִϸ��̼��� ���͵����� �������� �����ϴ� �Լ��Դϴ�.
	UFUNCTION()
		void SelfHealingAnimEnd();

public:
	RPC_FUNCTION(AHACKED_CRUSHER, RPCBothDieProcess)
	UFUNCTION()
	void RPCBothDieProcess();
	


};