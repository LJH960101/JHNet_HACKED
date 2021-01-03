#pragma once

#include "HACKED.h"
#include "InGame/Character/HackedCharacterAnimInstance.h"
#include "CRUSHER_AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FVoidNotifyDelegate);

class AHACKED_AI;

UCLASS()
class HACKED_API UCRUSHER_AnimInstance : public UHackedCharacterAnimInstance
{
	GENERATED_BODY()

public:
	UCRUSHER_AnimInstance();

	//NATIVE FUCNTIONS
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	/*-------------------------------------------------- ## Basic ## --------------------------------------------------*/
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimInstance", meta = (AllowPrivateAccess = "true"))
		class AHACKED_CRUSHER* player;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimInstance", meta = (AllowPrivateAccess = "true"))
		USphereComponent* checkSphere;
	void ValidatePlayerData();
	/*-------------------------------------------------- ## Attack ## --------------------------------------------------*/
private:
	//VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* attackMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bIsAttackState = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int currentAttackSectionNumber = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int currentComboNumber = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bCanDoNextAttackInput = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool isTracerActive = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		FVector attackTraceStartLoc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		FVector attackTraceEndLoc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		TArray<AHACKED_AI*> hitted;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bIsHit = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bDoOnce = false;
	UPROPERTY(VisibleAnywhere)
		USoundBase* enemyAttackSound1;
	UPROPERTY(VisibleAnywhere)
		USoundBase* enemyAttackSound2;
	UPROPERTY(VisibleAnywhere)
		USoundBase* enemyAttackSound3;
	UPROPERTY(EditAnywhere)
		USoundAttenuation* crusher_attack_attenuation;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		class UMaterialInterface* DC_CrusherAttack;





public:
	//FUNCTIONS
	UFUNCTION()
	void PlayAttackMontage();
	UFUNCTION()
	void DoNextAttack();
	UFUNCTION()
	void UpdateTracerLocation();
	UFUNCTION()
	void LineTracing();
	UFUNCTION()
	void GiveDamage(AHACKED_AI* damageTo);
	UFUNCTION()
	void PlayAttackHitSound();
	FORCEINLINE bool IsAttackMontagePlaying() { return Montage_IsPlaying(attackMontage)|| Montage_IsPlaying(ultimateAttackMontage); }
	FORCEINLINE bool IsAttackMontageActivated() { return Montage_IsActive(attackMontage) || Montage_IsActive(ultimateAttackMontage); }
	FORCEINLINE bool CanDoNextAttack() { return bCanDoNextAttackInput; }
	FORCEINLINE bool GetAttackState() { return bIsAttackState; }
	//NOTIFIES
	UFUNCTION()
		void AnimNotify_CheckNextAttack();
	UFUNCTION()
		void AnimNotify_StartShieldAttackHitTrace();
	UFUNCTION()
		void AnimNotify_StopShieldAttackHitTrace();
	/*-------------------------------------------------- ## Rocket Rush ## --------------------------------------------------*/
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
		bool bIsRocketRushing = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rocket Rush", meta = (AllowPrivateAccess = "true"))
	bool bIsRocketCharging = false;
public:
	FORCEINLINE void SetRocketRushingState(bool state) { bIsRocketRushing = state; }
	FORCEINLINE void SetRocketChargingState(bool state) { bIsRocketCharging = state; }
	/*-------------------------------------------------- ## Barrier Generator ## --------------------------------------------------*/
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Barrier Generator", Meta = (AllowPrivateAccess = true))
		bool bIsBarrierGenerateActive = false;
	UFUNCTION()
		void AnimNotify_SpawnBarrier();
	UFUNCTION()
		void AnimNotify_BarrierAnimationEnd();
public:
	FORCEINLINE bool GetBarrierGeneratorActive() const { return bIsBarrierGenerateActive; }
	FORCEINLINE void SetBarrierGeneratorActive(bool activeStats) { bIsBarrierGenerateActive = activeStats; }
	/*-------------------------------------------------- ## Punish Shield ## --------------------------------------------------*/
	FVoidNotifyDelegate PunishShieldDamageCheck;
	FVoidNotifyDelegate PunishShieldEndCheck;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SelfHealing")
		bool AnimIsPunishShield = false;

	UFUNCTION()
		void PlayPunishMontage();

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PunishShield, Meta = (AllowPrivateAccess = true))
		UAnimMontage* PunishMontage;

	UFUNCTION()
		void AnimNotify_PunishShieldDamaging();

	UFUNCTION()
		void AnimNotify_PunishShieldEnd();


	/*-------------------------------------------------- ## Ultimate ## --------------------------------------------------*/
public:
	//VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* ultimateAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		bool AnimIsAdrenalSurge = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ultimate", meta = (AllowPrivateAccess = "true"))
		bool bIsAdrenalSurge = false;
	//DELEGATES
	FVoidNotifyDelegate AdrenalSurgeCheck;
private:
	//NOTIFIES
	UFUNCTION()
		void AnimNotify_UltimateOn();
	UFUNCTION()
		void AnimNotify_GroundSmash();
	/*-------------------------------------------------- ## VitalityShield ## --------------------------------------------------*/
public:
	//VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VitalityShield", meta = (AllowPrivateAccess = "true"))
		bool AnimIsVitalityShield;
	//DELEGATES
	FVoidNotifyDelegate VitalityShieldCheck;
	FVoidNotifyDelegate VitalityShieldAnimEnd;
private:
	//NOTIFIES
	UFUNCTION()
		void AnimNotify_VitalityShieldCheck();
	UFUNCTION()
		void AnimNotify_VitalityShieldAnimEnd();

	/*-------------------------------------------------- ## SelfHealing ## --------------------------------------------------*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SelfHealing")
		bool AnimIsCrusherSelfHealing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SelfHealing")
		bool AnimIsCrusherSelfHealingFail;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SelfHealing")
		bool AnimIsCrusherDieProcess;

	FVoidNotifyDelegate CrusherDieEnd;

	FVoidNotifyDelegate CrusherSelfHealingEnd;

	UFUNCTION()
		void AnimNotify_CrusherDie();

	UFUNCTION()
		void AnimNotify_CrusherSelfHealingEnd();

	/*-------------------------------------------------- ## Dash ## --------------------------------------------------*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
		bool AnimIsCrusherDash;

	FVoidNotifyDelegate CrusherDashEnd;
private:
	UFUNCTION()
		void AnimNotify_DashEnd();

};
