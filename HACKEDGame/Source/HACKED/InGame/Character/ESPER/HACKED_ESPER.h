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
	//------------����Ÿ ��ǰ �� ���� ��� ���� ���� ������ ��ġ ������ ���̺� ������ ���� �Լ��Դϴ�. ----------------//
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

	// Aim ������ ���۴ϴ�.
	// ���ӿ������� ����մϴ�.
	void DisableAimWidget();

	// ��� ��ų�� BindAction�� �Ҵ�� ��ų ������ ���̹��� �Լ����� ���۵˴ϴ�. (SetupPlayerInputComponent ����)
	// ��� ��ų�� ����� ������ ������� ȣ��Ǿ� ���˴ϴ�. 

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

	bool bOnPrimaryAttack; // ������������ �˾Ƴ��� �����Դϴ�.

	// ������ �ߵ��մϴ�.
	UFUNCTION()
	void OnAttackCheck();

	//------------ Skill(1). Psychic Force ------------//
	// ��ų ���� : ������ �ֺ��� ���� ���������� ���ϰ� �о��, �ణ�� ���ظ� �ݴϴ�. 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicForce)
	UFUNCTION()
	void PsychicForce(); // Psychic Force ��ų�� ����ϱ� ���� ���� �Լ��Դϴ�. <Delegate ȣ��> 

	UFUNCTION()
	void PsychicForceDamaging(); // HACKED_AI�� ���� HACKEDLaunchCharacter�Լ��� ���� ���� �о�� ���ظ� �ִ� �Լ��Դϴ�. <Delegate ȣ��>

	UFUNCTION()
	void PsychicForceStun(); // ���ظ� ���� AI���� ��õ��� ������ �־��ִ� �Լ��Դϴ�. <���� ȣ��> 

	UFUNCTION()
	void OnPsychicForceEnd(); // �ش� ��ų�� ���� �⺻ State�� ���ư��� ���� ���� �Լ��Դϴ�. <Delegate ȣ��>

	TArray<class AHACKED_AI*> PsychicForceCheckAI;

public:
	UFUNCTION()
	void PsychicForceDamageChange(float plusDamage);



	//------------ Skill(M). Psychic Drop ------------//
	// ��ų ���� : �����۰� ���̹� ���� ������Ʈ�� ������ ��ü�� ������ �Ͷ߷� ���ظ� ������ ��ų�Դϴ�. (�ִ� ��Ÿ� 15m) 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicDrop)
	UFUNCTION()
	void PsychicDrop(); // Psychic Force ��ų�� ����ϱ� ���� ���� �Լ��Դϴ�. 

	UFUNCTION()
	void PsychicDropSpawnEmitter(); // �÷��̾ ������ ��ǥ�� ����Ʈ�� ��ȯ�ϱ� ���� �Լ� (�ش� �Լ� �ڿ� AfterDamage�� ���� PsychicDropDamaging�� ȣ��˴ϴ�.) 

	RPC_FUNCTION(AHACKED_ESPER, RPCSpawnDropEmitter, FVector, FVector)
	UFUNCTION()
	void RPCSpawnDropEmitter(FVector targetPoint, FVector targetRotation);

	UFUNCTION()
	void OnPsychicDropDamaging(); // ������ ����Ʈ�� ���� ���ظ� �ֱ� ���� �Լ��Դϴ�.

	UFUNCTION()
	void OnPsychicDropEnd(); // �ش� ��ų�� ���� �⺻ State�� ���ư��� ���� ���� �Լ��Դϴ�.

	class AEsper_PsychicDrop* lastPhychicDrop;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystem* PS_PsychicDropCharge;

public:
	UFUNCTION()
		void PsychicDropDamageChange(float plusDamage);


	//------------ Skill(2). Psychic ShockWave ------------//
	// ��ų ���� : �����۰� ���濡 ������ ���������� �߻��մϴ�. �¿� ȸ�������� �������� ������ �ֽ��ϴ�. 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicShockWave)
	UFUNCTION()
	void PsychicShockWave(); // Psychic ShockWave ��ų�� ����ϱ� ���� ���� �Լ��Դϴ�.

	UFUNCTION()
	void ShockWaveSpawnEmitter(); // ����ű ��ũ���̺� ����Ʈ�� ��ȯ�մϴ�. 

	UFUNCTION()
	void ShockWaveDamagingStart(); // ����ű ��ũ ���̺� ����¡ ��ŸƮ 

	UFUNCTION()
	void ShockWaveDamaging(); // ������ ������ ������ ��Ʈ �������� �����ϴ�. 

	UFUNCTION()
	void ShockWaveDamagingEnd(); // ����ű ��ũ���̺� ����Ʈ�� �����ϸ�, ����¡�� �����մϴ�. 

	UFUNCTION()
	void ShockWaveSetTarget(); // Beam Data Type�� ����ű ��ũ���̺� ����Ʈ�� ������ ����� ���ÿ� ����Ʈ�� ĳ������ ȸ���� ����ȭ�մϴ�. 

	UFUNCTION()
	void ShockWaveEnd(); // �ش� ��ų�� ���� �⺻ State�� ���ư��� ���� ���� �Լ��Դϴ�.

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
	// ��ų ���� : �����۰� �ǵ带 �����մϴ�. �ݰ� 7m ���� ũ���Ű� ������� ũ���ŵ� �ǵ� ȿ���� ���� �޽��ϴ�.
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
	// ��ų ���� : ������ �ֺ��� ���� ������ ���ø��� ������ ���������� ���� ������ �Ƚ��ϴ�. 
private:
	RPC_FUNCTION(AHACKED_ESPER, PsychicOverDrive)
	UFUNCTION()
	void PsychicOverDrive(); // ������ �ñر�(Psychic OverDrive)�� ó�� ���� ���� �Լ��Դϴ�.

	UFUNCTION()
	void PsychicOverDriveCamReturn();

	UFUNCTION()
	void PsychicOverDriveEndAction(); // ���ε��Լ��� Tick���� ���ø� ���� ������ �Ƚ��ϴ�. 

	UFUNCTION()
	void PsychicOverDriveDamaging(); // ������ ���� ������ ���ظ� �����ϴ�. 

	UFUNCTION()
	void PsychicOverDriveEnd(); // �ش� ��ų�� ���� �⺻ State�� ���ư��� ���� ��ġ �����Լ��Դϴ�. 

	UPROPERTY()
	class UMaterial* PrimaryPartsMaterial;

	UPROPERTY()
	class UMaterial* UltimatePartsMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* PrimaryMaterialInst;

	UPROPERTY()
	class UMaterialInstanceDynamic* UltimateMaterialInst;

	FTimerHandle UltimateDamagingTimer; // �ñرⰡ ���� �� ���� ������ ���ظ� ���� ������ �Լ��� �����Ű�� �Լ��Դϴ�.
	TArray<class AHACKED_AI*> UltimateCheckedAI; // ���� ������ ���ظ� ������ ���� ���� �����ϴ� Array
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
	// �ڰ� ġ�� ���¿� �����ϴ� �Լ��Դϴ�.
	void RPCSelfHealing();

	// �ڰ� ġ���� �������� �����ϴ� �Լ��Դϴ�.
	UFUNCTION()
	void SelfHealingEnd();

	// �ڰ� ġ�� �ִϸ��̼��� ���͵����� �������� �����ϴ� �Լ��Դϴ�.
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
	void CameraBackToOrigin(); // ī�޶� �⺻ State ���·� �����ϴ�. 

public:
	// Camera Shake Bp ���� 
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

	// �ñر� ���� ���� �ߵ� ����
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
