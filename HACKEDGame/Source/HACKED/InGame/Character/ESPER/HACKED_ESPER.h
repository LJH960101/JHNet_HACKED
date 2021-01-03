// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/Character/HACKEDCharacter.h"
#include "ESPER_StatComponent.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "HACKED_ESPER.generated.h"

UCLASS()
class HACKED_API AHACKED_ESPER : public AHACKEDCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHACKED_ESPER();
	virtual void Possessed(AController* NewController) override;
	virtual void UnPossessed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = SkillSystem)
	virtual int GetSkillRate(int skill);

	UFUNCTION(BlueprintPure, Category = EsperPrimaryAttack)
	bool GetOnPrimaryAttack() { return bOnPrimaryAttack; }

	UFUNCTION(BlueprintImplementableEvent, Category = Aim)
	void OnHitAim(bool IsCritical);

	bool IsOnSelfHealing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


public:
	void InitEsperStat();

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UESPER_StatComponent* Esper_Stat;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void PlaySkill1Sound(FVector location);

	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void PlaySkill2Sound(FVector location);


private:
	//------------지스타 출품 및 스팀 출시 위한 최종 에스퍼 수치 데이터 테이블 적용을 위한 함수입니다. ----------------//
	FHACKED_ESPER_Stat esperStat;
	float GetMaxHp();
	float GetprimaryAttackDamage();
	float GetPASPDamage();
	float GetPASPRange();
	float GetPASpeed();
	float GetPsychicForceDamage();
	float GetPsychicForceRange(); 
	float GetPsychicDropDamage();
	float GetPsychicDropRange();
	float GetPsychicWaveDamage();
	float GetPsychicWaveRange();

	// Aim 위젯을 없앱니다.
	// 게임오버에서 사용합니다.
	void DisableAimWidget();

	// 모든 스킬은 BindAction에 할당된 스킬 고유의 네이밍의 함수에서 시작됩니다. (SetupPlayerInputComponent 참고)
	// 모든 스킬은 헤더에 나열된 순서대로 호출되어 사용됩니다. 

private:
	//------------ Link Beam ------------//

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LinkBeam, Meta = (AllowPrivateAccess = true))
		class UParticleSystemComponent* PC_LinkBeam = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* PS_LinkBeam;

public:

	RPC_FUNCTION(AHACKED_ESPER, EsperIntroSkipStart)
	UFUNCTION()
	void EsperIntroSkipStart();

	UFUNCTION()
	void EsperIntroSkip();

	RPC_FUNCTION(AHACKED_ESPER, EsperIntroSkipStop)
	UFUNCTION()
	void EsperIntroSkipStop();

	UFUNCTION(BlueprintImplementableEvent)
	void EsperTurnOffSkipUI();


private:
	//------------ Esper Primary Attack ------------//

	RPC_FUNCTION(AHACKED_ESPER, Esper_Attack)
	void Esper_Attack();

	RPC_FUNCTION(AHACKED_ESPER, Esper_AttackEnd)
	void Esper_AttackEnd();

	bool bOnPrimaryAttack; // 공격중인지를 알아내는 변수입니다.

	// 공격을 발동합니다.
	UFUNCTION()
	void OnAttackCheck();

	//------------ Skill(1). Psychic Force ------------//
	// 스킬 설명 : 에스퍼 주변의 적을 염동력으로 강하게 밀어내며, 약간의 피해를 줍니다. 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicForce)
	UFUNCTION()
	void PsychicForce(); // Psychic Force 스킬을 사용하기 위한 시작 함수입니다. <Delegate 호출> 

	UFUNCTION()
	void PsychicForceDamaging(); // HACKED_AI에 만든 HACKEDLaunchCharacter함수를 통해 적을 밀어내며 피해를 주는 함수입니다. <Delegate 호출>

	UFUNCTION()
	void PsychicForceStun(); // 피해를 입은 AI에게 잠시동안 스턴을 넣어주는 함수입니다. <개별 호출> 

	UFUNCTION()
	void OnPsychicForceEnd(); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 엔드 함수입니다. <Delegate 호출>

	TArray<class AHACKED_AI*> PsychicForceCheckAI;

public:
	UFUNCTION()
	void PsychicForceDamageChange(float plusDamage);



	//------------ Skill(M). Psychic Drop ------------//
	// 스킬 설명 : 에스퍼가 에이밍 상의 오브젝트에 에너지 구체를 붙인후 터뜨려 피해를 입히는 스킬입니다. (최대 사거리 15m) 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicDrop)
	UFUNCTION()
	void PsychicDrop(); // Psychic Force 스킬을 사용하기 위한 시작 함수입니다. 

	UFUNCTION()
	void PsychicDropSpawnEmitter(); // 플레이어가 지정한 목표에 이펙트를 소환하기 위한 함수 (해당 함수 뒤엔 AfterDamage를 위해 PsychicDropDamaging이 호출됩니다.) 

	RPC_FUNCTION(AHACKED_ESPER, RPCSpawnDropEmitter, FVector, FVector)
	UFUNCTION()
	void RPCSpawnDropEmitter(FVector targetPoint, FVector targetRotation);

	UFUNCTION()
	void OnPsychicDropDamaging(); // 생성된 이펙트에 범위 피해를 주기 위한 함수입니다.

	UFUNCTION()
	void OnPsychicDropEnd(); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 엔드 함수입니다.

	class AEsper_PsychicDrop* lastPhychicDrop;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystem* PS_PsychicDropCharge;

public:
	UFUNCTION()
		void PsychicDropDamageChange(float plusDamage);


	//------------ Skill(2). Psychic ShockWave ------------//
	// 스킬 설명 : 에스퍼가 전방에 강력한 에너지빔을 발사합니다. 좌우 회전만으로 데미지를 입힐수 있습니다. 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicShockWave)
	UFUNCTION()
	void PsychicShockWave(); // Psychic ShockWave 스킬을 사용하기 위한 시작 함수입니다.

	UFUNCTION()
	void ShockWaveSpawnEmitter(); // 싸이킥 쇼크웨이브 이펙트를 소환합니다. 

	UFUNCTION()
	void ShockWaveDamagingStart(); // 싸이킥 쇼크 웨이브 데미징 스타트 

	UFUNCTION()
	void ShockWaveDamaging(); // 전방의 복수의 적에게 도트 데미지를 입힙니다. 

	UFUNCTION()
	void ShockWaveDamagingEnd(); // 싸이킥 쇼크웨이브 이펙트를 삭제하며, 데미징을 종료합니다. 

	UFUNCTION()
	void ShockWaveSetTarget(); // Beam Data Type인 싸이킥 쇼크웨이브 이펙트의 방향을 맞춤과 동시에 이펙트와 캐릭터의 회전을 동기화합니다. 

	UFUNCTION()
	void ShockWaveEnd(); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 엔드 함수입니다.

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* ShockWave = nullptr;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* ShockWaveHit = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PA_ShockWave;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PA_ShockWaveHit;
public:
	UFUNCTION()
		void PsychicShockWaveDamageChange(float plusDamage);

	//------------ Skill(3). Psychic Shield ------------//
	// 스킬 설명 : 에스퍼가 실드를 생성합니다. 반경 7m 내에 크러셔가 있을경우 크러셔도 실드 효과를 같이 받습니다.
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicShield)
	UFUNCTION()
	void PsychicShield();

	RPC_FUNCTION(AHACKED_ESPER, RPCCreatePsychicShield, bool)
	UFUNCTION()
	void RPCCreatePsychicShield(bool isBoth);

	UFUNCTION()
	void PsychicShieldOn();

	RPC_FUNCTION(AHACKED_ESPER, RPCDestroyShield)
	UFUNCTION()
	void RPCDestroyShield();

	UFUNCTION()
	void PsychicShieldAnimEnd();

public:
	UFUNCTION()
		void PsychicShieldDestroy();

	UFUNCTION()
	void PsychicShieldTimeChange(float plusTime);


	//------------ Ultimate(U). Psychic OverDrive ------------//
	// 스킬 설명 : 에스퍼 주변의 적을 제압해 들어올린후 강력한 염동력으로 적을 땅으로 꽂습니다. 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicOverDrive)
	UFUNCTION()
	void PsychicOverDrive(); // 에스퍼 궁극기(Psychic OverDrive)의 처음 상태 설정 함수입니다.

	UFUNCTION()
	void PsychicOverDriveCamReturn();

	UFUNCTION()
	void PsychicOverDriveEndAction(); // 바인드함수와 Tick에서 들어올린 적을 땅으로 꽂습니다. 

	UFUNCTION()
	void PsychicOverDriveDamaging(); // 땅으로 꽂힌 적에게 피해를 입힙니다. 

	UFUNCTION()
	void PsychicOverDriveEnd(); // 해당 스킬이 끝나 기본 State로 돌아가기 위한 수치 조정함수입니다. 

	UPROPERTY()
	class UMaterial* PrimaryPartsMaterial;

	UPROPERTY()
	class UMaterial* UltimatePartsMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* PrimaryMaterialInst;

	UPROPERTY()
	class UMaterialInstanceDynamic* UltimateMaterialInst;

	FTimerHandle UltimateDamagingTimer; // 궁극기가 끝난 뒤 내려 박힐때 피해를 직접 입히는 함수를 실행시키는 함수입니다.
	TArray<class AHACKED_AI*> UltimateCheckedAI; // 꽂힌 적에게 피해를 입히기 위해 적을 저장하는 Array
public:
	UFUNCTION(BlueprintImplementableEvent, Category = Ultimate)
	void OnTranslucent(bool IsTranslucent);

	UFUNCTION(BlueprintImplementableEvent, Category = Ultiamte)
	void UltimateDecal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UMaterialInterface* DC_EsperUltimateDecal;

	// SelfHealing ------------------------------------
private:
	RPC_FUNCTION(AHACKED_ESPER, RPCSelfHealing)
	UFUNCTION()
	// 자가 치유 상태에 진입하는 함수입니다.
	void RPCSelfHealing();

	// 자가 치유가 끝났을때 진입하는 함수입니다.
	UFUNCTION()
	void SelfHealingEnd();

	// 자가 치유 애니메이션후 복귀동작의 마지막에 진입하는 함수입니다.
	UFUNCTION()
	void SelfHealingAnimEnd();

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* PC_EsperSelfHeal = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* PS_EsperSelfHeal;

public:

	RPC_FUNCTION(AHACKED_ESPER, RPCBothDieProcess)
	UFUNCTION()
	void RPCBothDieProcess();

private:
	// Dash ------------------------------------
	RPC_FUNCTION(AHACKED_ESPER, PsychicDash)
	UFUNCTION()
	void PsychicDash();

	UFUNCTION()
	void PsychicDashing();

	RPC_FUNCTION(AHACKED_ESPER, PsychicDashFinish)
	UFUNCTION()
	void PsychicDashFinish();


	// Camera ------------------------------------
	void CameraBackToOrigin(); // 카메라를 기본 State 상태로 돌립니다. 

public:
	// Camera Shake Bp 연동 
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_EsperPrimary;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_PsychicOverDrive;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_PsychicOverDriveEnd;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_PsychicShockWave;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_PsychicForce;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_PsychicDrop;


private:
	//------------------------------------------ Esper Stat ------------------------------------------//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EsperPrimaryStat, Meta = (AllowPrivateAccess = true))
	float _esperMaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EsperPrimaryStat, Meta = (AllowPrivateAccess = true))
	float _esperMaxWalkSpeed;


private:
	//------------------------------------------ Esper Primary Attack ------------------------------------------//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	float _primaryAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	float _primaryAttackSPDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	float _primaryAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	float _primaryAttackSPRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	float _primaryAttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	float _primaryAttackLifeTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = PrimaryAttack, Meta = (AllowPrivateAccess = true))
	bool bIsPrimaryAttacking;

	//------------------------------------------ Skill 1 PsychicForce ------------------------------------------//
private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill1_PsychicForce, Meta = (AllowPrivateAccess = true))
	bool bIsPsychicForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill1_PsychicForce, Meta = (AllowPrivateAccess = true))
	float _psychicForceRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill1_PsychicForce, Meta = (AllowPrivateAccess = true))
	float _psychicForceDamage;

	FTimerHandle PsychicForceStunDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill1_PsychicForce, Meta = (AllowPrivateAccess = true))
	float _stunDelayTime;


	//------------------------------------------ Skill MouseRight PsychicDrop ------------------------------------------//
private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill2_PsychicDrop, Meta = (AllowPrivateAccess = true))
	bool bIsPsychicDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill2_PsychicDrop, Meta = (AllowPrivateAccess = true))
	float _psychicDropSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill2_PsychicDrop, Meta = (AllowPrivateAccess = true))
	float _psychicDropRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill2_PsychicDrop, Meta = (AllowPrivateAccess = true))
	float _psychicDropDamageRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill2_PsychicDrop, Meta = (AllowPrivateAccess = true))
	float _psychicDropDamage;

	float PsychicDropDamageCount;

	bool bActorHitCheck;
	FVector PsychicDropSavePos;
	FVector PsychicDropStartVec;
	FVector PsychicDropEndVec;
	FVector FollowCamVec;
	FVector WorldCamVec;
	FVector PsychicDropHitLocation;

	//------------------------------------------ Skill 2 PsychicShockWave ------------------------------------------//
private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	bool bIsPsychicShockWave;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _shockWaveRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _shockWaveRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _shockWaveDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _shockWaveTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _shockWaveMaxTime;

	FVector ShockWaveSpawnLocation;

	FTimerHandle ShockWaveTimerHandle;

	//------------------------------------------ Skill 3 PsychicShield ------------------------------------------//
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	bool bIsPsychicShield;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _psychicShieldDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _psychicShieldAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill3_PsychicShockWave, Meta = (AllowPrivateAccess = true))
	float _psychicShieldTime;

	FTimerHandle PsychicShieldTimerHandle;

	//------------------------------------------ Shift PsychicDash ------------------------------------------//
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Dash_PsychicDash, Meta = (AllowPrivateAccess = true))
	bool bIsPsychicDash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash_PsychicDash, Meta = (AllowPrivateAccess = true))
	float _psychicDashDistance = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dash_PsychicDash, Meta = (AllowPrivateAccess = true))
	float _psychicDashPower;

	FVector DashStartPos;

	FVector DashDirection;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PS_PsychicDash;



	//------------------------------------------ Ultimate PsychicOverDrive ------------------------------------------//
public:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Ultimate_PsychicOverDrive, Meta = (AllowPrivateAccess = true))
	bool bIsPsychicOverDrive = false;

private:

	// 궁극기 띄우는 판정 발동 여부
	bool bOnPsychicOverDriveHovering;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate_PsychicOverDrive, Meta = (AllowPrivateAccess = true))
	float _psychicOverDriveRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate_PsychicOverDrive, Meta = (AllowPrivateAccess = true))
	float _psychicOverDriveDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate_PsychicOverDrive, Meta = (AllowPrivateAccess = true))
	float _ultimateDropDelay;

	FVector OverDriveSavePos;

	FTimerHandle UltimateTimerHandle;
	float OverDriveTimeCount;



private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Particle, Meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* PA_PsychicDrop;

	UPROPERTY()
		TSubclassOf<class AEsper_PA_EnergyBall> EnergyBallClass;

	UPROPERTY()
		TSubclassOf<class AEsper_PsychicDrop> PsychicTrailClass;

	UPROPERTY()
	TSubclassOf<class UUserWidget> WG_Aim_Class;

	UPROPERTY()
	class UEsper_AnimInstance* Esper_Anim;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Widget")
	class UUserWidget* WG_Aim = nullptr;
};
