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
	// Aim 위젯을 없앱니다.
	// 게임오버에서 사용합니다.
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
	//기본 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	float normalDamage = 50.0f;
	//크리티컨 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		float criticalDamage = 80.0f;
	//그라운드 스메쉬 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		float groundSmashRadius = 500.0f;
	//그라운드 스메쉬 데미지
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
	//로켓 러쉬 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MaxRushDistance = 2000.0f;
	//로켓 러쉬 계산 거리
	UPROPERTY()
		float rr_RushDistance = 0.0f;
	//로켓 러쉬 현재 차징 시간
	UPROPERTY()
		float rr_Charging = 0.0f;
	//로켓 러쉬 최대 차징 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MaxCharging = 2.0f;
	//로켓 러쉬 최소 차징 시간 (이 시간을 못넘을 시 스킬 즉시 종료)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MinCharging = 0.4f;
	//적을 밀어내는 힘
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_PushPower = 3000.0f;
	//앞으로 로켓 러쉬하는 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_RushSpeed = 3000.0f;
	//로켓 러쉬가 종료될 최소 Velocity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_MinVelocity = 700.0f;
	//로켓 러쉬에 맞을 시 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_Damage = 100.0f;
	//초반 Velocity가 MinVelocity를 넘지 못하는 경우가 있어 추가하는 검진 대기시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_checkWaitTime = 0.0f;
	//최대 검진 대기시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		float rr_checkWaitMaxTime = 0.15f;
	//차징 중인가
	UPROPERTY()
		bool bIsRocketRushChargingState = false;
	//로켓 러쉬 중인가
	UPROPERTY()
		bool bIsRocketRushLaunchingState = false;
	//로켓 러쉬한 거리 계산을 위한 위치 저장 변수
	UPROPERTY()
		FVector rr_StartLocation;
	//로켓 러쉬할 방향 저장 변수
	UPROPERTY()
		FVector rr_ForwardVector;
	//맞은 적들을 담을 구조체
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
	UFUNCTION() 	// 자가 치유 상태에 진입하는 함수입니다.
		void RPCSelfHealing();

	// 자가 치유가 끝났을때 진입하는 함수입니다.
	UFUNCTION()
		void SelfHealingEnd();

	// 자가 치유 애니메이션후 복귀동작의 마지막에 진입하는 함수입니다.
	UFUNCTION()
		void SelfHealingAnimEnd();

public:
	RPC_FUNCTION(AHACKED_CRUSHER, RPCBothDieProcess)
	UFUNCTION()
	void RPCBothDieProcess();
	


};