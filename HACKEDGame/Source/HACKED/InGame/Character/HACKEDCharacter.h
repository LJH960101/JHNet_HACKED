// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/Network/ReplicatableVar.h"
#include "HACKED_SkillSystemComponent.h"
#include "InGame/Stat/PlayerDamageEvent.h"
#include "NetworkModule/NetworkData.h"
#include "NetworkModule/Serializer.h"
#include "InGame/Interface/Networkable.h"
#include "HACKEDCharacter.generated.h"

class UHACKED_SkillSystemComponent;
class AHackingPanel;

UCLASS(config=Game)
class AHACKEDCharacter : public ACharacter, public INetworkable
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	bool bWithoutSP = false;

	// �Է¿� ���� 3���� ������ �ѹ��� �Է��ϴ� �Լ�.
	void SetEnableInput(bool canMove = true, bool canTurn = true, bool canAttack = true);
protected:
	// ������ �� �ִ����� �����ϴ� ����
	bool _bCanMove = true;
	// ī�޶� ȸ�� ��ų �� �ִ����� �����ϴ� ����
	bool _bCanTurn = true;
	// ������ �� �� �ִ����� �����ϴ� ����.
	bool _bCanAttack = true;

	class AHACKEDInGameManager* HACKEDInGameManager;

	// ȸ���ӵ��� �������ִ� �Լ� 
	void SetTurnRate(float baseTurnRate = 20.0f, float baseLookUpRate = 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UHACKED_CameraArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network, meta = (AllowPrivateAccess = "true"))
	class UNetworkTransformCP* NetTransformCP;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UHpComponent* HpComponent;

	//F1 key Info Widget Setup
	UPROPERTY(VisibleAnywhere, Category = F1)
	TSubclassOf<class UUserWidget> WG_Info_Class;
	UPROPERTY(VisibleAnywhere, Category = F1)
	class UUserWidget* WG_Info = nullptr;

	void ShowInfoWidget();
	void CloseInfoWidget();

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Sound")
	void PlayHitSound();

	// ���� ���� ���൵�� ��Ÿ���ϴ�.
	UFUNCTION(BlueprintPure, Category = "HP")
	float GetSelfHealingRate();

	UHpComponent* GetHPComponent() { return HpComponent; }

	
protected:
	// ---------------------------------------- ��Ÿ�� ���� ������ bool Ÿ�� �����Դϴ�. (�ܺ� ��� ���� ����) ---------------------------------------- // 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bIsM2CoolDown = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bIsFirstCoolDown = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bIsSecondCoolDown = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bIsThirdCoolDown = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bIsDashCoolDown = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bCanUltimate;

public:
	// ---------------------------------------- ��ų ��Ȱ��ȭ�� bool Ÿ�� �����Դϴ�. (�ܺ� ��� ����) ---------------------------------------- // 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SkillCoolTimeSystem)
	bool bCanPrimary = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialSkillDisable)
	bool bCanM2Skill = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialSkillDisable)
	bool bCanFirstSkill = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialSkillDisable)
	bool bCanSecondSkill = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialSkillDisable)
	bool bCanThirdSkill = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialSkillDisable)
	bool bCanDashSkill = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialSkillDisable)
	bool bUltimateDisable = true;

	UFUNCTION()
	void SkillDisable(bool canPrimary = true, bool canM2 = true, bool canFirst = true, bool canSecond = true, bool canThird = true, bool canDash = true, bool canUltimate = true);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate)
	bool bIsAdrenalSurge = false;


private:
	CReplicatableVar<float> _currentSpeed;
	CReplicatableVar<bool> _bOnJump;
	CReplicatableVar<float> _currentForwardSpeedRate;
	CReplicatableVar<float> _currentRightSpeedRate;

public:
	AHACKEDCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintPure, Category = "Character Movement", meta = (BlueprintThreadSafe))
	float GetForwardSpeedRate();
	UFUNCTION(BlueprintPure, Category = "Character Movement", meta = (BlueprintThreadSafe))
	float GetRightSpeedRate();
	virtual void Possessed(AController* NewController);
	bool HasAuthority();

public: 
	//--------------Intro & Tutorial--------------//

	UFUNCTION(BlueprintPure, Category = SkipIntro)
	float CurrentIntroTime();

	UFUNCTION(BlueprintPure, Category = SkipTutorial)
	bool GetWantToSkipTuto();

	UFUNCTION(BlueprintPure, Category = SkipTutorial)
	bool GetSkipTutoCallCheck();

	RPC_FUNCTION(AHACKEDCharacter, WantToSkipTuto, bool, bool)
	UFUNCTION(BlueprintCallable, Category = TutorialSkip)
	void WantToSkipTuto(bool skipCheck, bool firstCheck = false);
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkipIntro)
	float currentIntroSkipTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkipIntro)
	float maxtIntroSkipTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkipIntro)
	bool bCanSkipIntro = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkipTutorial)
	bool bSkipTutorial= false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SkipTutorial)
	bool bSkipFunctionUsedCheck = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialStat)
	bool bIsNowOnTutorial = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialStat)
	bool bIsNowOnCooperationTuto = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialStat)
	int32 DashCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = TutorialStat)
	int32 BarrierHitCount = 0;
	
public:
	FHACKED_SkillSystem_Stat* spStat;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SelfHealingState, Meta = (AllowPrivateAccess = true))
	bool bIsSelfHealing = false;

protected:

	FTimerHandle SelfHealingTimer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfHealingState, Meta = (AllowPrivateAccess = true))
	float selfHealingTime;

	FTimerHandle UltimateTimerHandle;
	FTimerHandle UltimateTimerCheckHandle;

	float _ultimateTime;
	float _maxUltimateTime;

	FTimerHandle M2SkillTimerHandle;
	FTimerHandle FirstSkillTimerHandle;
	FTimerHandle SecondSkillTimerHandle;
	FTimerHandle ThirdSkillTimerHandle;
	FTimerHandle DashSkillTimerHandle;


protected:
// ==================== Skill Cool Time SyStem ==================== // 
	void SkillCoolApplySystem(const ESkillNumberType& skillNum, bool canSkill);

	void M2SkillTimerCheck();
	float _m2SkillTime = 0;
	int _maxM2SkillTime;

	void FirstSkillTimerCheck();
	float _firstSkillTime = 0;
	int _maxFirstSkillTime;

	void SecondSkillTimerCheck();
	float _secondSkillTime = 0;
	int _maxSecondSkillTime;

	void ThirdSkillTimerCheck();
	float _thirdSkillTime = 0;
	int _maxThirdSkillTime;

	void DashSkillTimerCheck();
	float _dashSkillTime = 0;
	int _maxDashSkillTime;

// ====================== Ultimate Damage Meter System ====================== // 
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ultimate, Meta = (AllowPrivateAccess = true))
	float maxUltimateGauge = 3000.0f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Ultimate, Meta = (AllowPrivateAccess = true))
	float currentUltimateGauge = 0.0f;

	UFUNCTION()
	float GetCurrentUltimateGauge();

	void UltimateGauageSystem();

	void UltimateGaugeReset();

public:

// ========================Sp System=============================================== //
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UHACKED_SkillSystemComponent* SpCost_Stat;

	// Init Hero Skill Cost Stat
	void InitSpCostStatByType(const EHackedHeroType& type);

	// Skill M2 Cost
	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetM2SkillCost();

	// Skill 1 Cost
	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetFirstSkillCost();

	// Skill 2 Cost
	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetSecondSkillCost();

	// Skill 3 Cost
	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetThirdSkillCost();

	// Skill Dash Cost
	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetDashSkillCost();

	// Ultimate Gauge Cost
	float GetUltimateMaxGauge();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SpSytem)
	int32 CurrentSp = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SpSytem)
	int32 MaxSp = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SpSystem)
	int32 UltimateSp = 0;

	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetM2SkillRate();

	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetFirstSkillRate();

	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetSecondSkillRate();

	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetThirdSkillRate();

	UFUNCTION(BlueprintPure, Category = SkillSystem)
	int GetDashSkillRate();
	

	// SP �ý��� �� �ñر� �ý��� =====================================

	// SP �ʱ�ȭ
	void SpSytemSetUpState();
	
	// SP ȹ��
	void SpSystemStartState();

	// SP ���
	// ���� : �����ߴ��� �����ߴ���?
	bool SpReduceApplyState(const ESkillNumberType& skillNum);

	void UltimateApplyState(float ultimateTime, bool canUltimate = false);

	void UltimateTimerCheck();

	UFUNCTION(BlueprintPure, Category = SpSystem)
	float GetSPRate();
	UFUNCTION(BlueprintPure, Category = SpSystem)
	int32 GetSp();

	// ������ ó���� ���� �����ϴ� SP
	int32 _savedSP;

	RPC_FUNCTION(AHACKEDCharacter, _RPCSaveSP, int32);
	void _RPCSaveSP(int32 sp);

	RPC_FUNCTION(AHACKEDCharacter, _RPCRefreshSP, int32);
	void _RPCRefreshSP(int32 sp);

//==================================================================================


public:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
	class UParticleSystem* PS_VitalityShield;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
	class UParticleSystem* PS_VitalityShieldEnd;

	UPROPERTY(VisibleAnywhere)
	class UParticleSystemComponent* VitalityShieldComponent = nullptr;

	UFUNCTION()
	void VitalityShieldSpawnEmitter();

	UFUNCTION()
	void VitalityShieldEndEmitter();

public:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* PS_PsychicShieldStart;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* PS_OnPsychicShield;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = VitalityShield, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* PS_PsychicShieldEnd;

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* PC_OnPsychicShield = nullptr;

	UFUNCTION()
	void PsychicShieldSpawnEmitter();

	RPC_FUNCTION(AHACKEDCharacter, PsychicShieldEndEmitter)
	UFUNCTION()
	void PsychicShieldEndEmitter();

public:

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* PC_EsperAdrenalSurge = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = AdrenalSurge, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* PS_EsperAdrenalSurge;

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* PC_CrusherAdrenalSurge = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = AdrenalSurge, Meta = (AllowPrivateAccess = true))
		class UParticleSystem* PS_CrusherAdrenalSurge;

	UFUNCTION()
	void AdrenalSurgeSpawnEmitter(bool selfCheck);

	UFUNCTION()
	void AdrenalSurgeEndEmitter();

public:



public:
	// ������ ������ ȣ��Ǵ� �Լ�.
	virtual void Reconnected() override;

public:

	UFUNCTION(BlueprintPure, Category = Stat)
	float GetHpRate();

	UFUNCTION(BlueprintPure, Category = Stat)
	float GetShieldRate();

	UFUNCTION(BlueprintPure, Category = Stat)
	float GetUltimateRate();

private:
	RPC_FUNCTION(AHACKEDCharacter, RPCSetHPAndShield, float, float)
	UFUNCTION()
	void RPCSetHPAndShield(float hp, float shield);

public:

	// ī�޶� �¿� Turn Speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	// ī�޶� ���� Turn Speed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// Used in Animation Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float currentSpeedRate = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bOnMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Immortal)
	bool bIsImmortal = false;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	class UNetworkBaseCP* NetBaseCP;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void Jump();
	void StopJumping();

public:
	UFUNCTION(BlueprintPure, Category = "HACKEDCharacter")
	bool OnJump();

public:
	FORCEINLINE class UHACKED_CameraArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetCanAttack(bool _state) { _bCanAttack = _state; }

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Damaged")
	void OnDamaged();

	// �÷��̾� ������ ���� 
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	void PressHackingPanel();
	void SetHackingPanel(AHackingPanel* hackingPanel);
	class AHackingPanel* currentHackingPanel;

private:
	// ���ӿ��� ���� �Լ��� ����
	// ���� ���� UI�� �߱� �ð��ϴ� �ð�
	float _gameOverFadeInTime = 3.0f;
	// ȭ���� FadeOut �Ǵ� �ð�
	float _gameOverFadeOutTime = 3.0f;
	FTimerHandle GameOverTimer;
	FTimerHandle BackToLobbyTimer;

protected:
	void ShowGameOver();
	void BackToLobby();

	// �κ�� �̵��ϴ� Ÿ�̸Ӱ� �������� ȣ��Ǵ� �Լ�.
	UFUNCTION()
	void OnBackToLobby();
};

