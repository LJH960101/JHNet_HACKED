// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/AI/HACKED_AI.h"
#include "SecurityRobot_SLS.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSLSAttackEndDelegate);

UCLASS()
class HACKED_API ASecurityRobot_SLS : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASecurityRobot_SLS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	//virtual void Reconnect() override;
	//virtual void Reconnected() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void OnSpawn() override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* SpringArm;

	void SLSAttack();
	FOnSLSAttackEndDelegate OnSLSAttackEnd;

private:


	UFUNCTION()
	void OnSLSAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void SLSAttackStartComboState();
	void SLSAttackEndComboState();

	void SLSAttackDamaging();

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bIsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bCanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		bool bIsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
		int32 MaxCombo;

	UPROPERTY()
		class USR_SLS_AnimInstance* SLS_Anim;
};
