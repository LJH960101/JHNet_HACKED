// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Components/ActorComponent.h"
#include "HpComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FVoidDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HACKED_API UHpComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	float _hp;
	float _maxHp;



	float _shield;
	float _maxShield;

public:	
	// Sets default values for this component's properties
	UHpComponent();
	bool shieldCheck;
	// HP를 MAX HP만큼 회복 시킵니다.
	// AI 리스폰시 사용합니다.
	void SetToMaxHP();
	void SetHP(float hp);
	void SetShield(float shield);
	void SetHPAndShield(float hp, float shield);

	void SetMaxHP(float maxHP);
	float GetMaxHp();
	float GetHP();

	void SetMaxShield(float maxShield);
	void SetShieldAmount(float setShield);
	float GetMaxShield();
	float GetShieldAmount();

	float TakeDamage(float damage);

public:
	FVoidDelegate OnSetChangeShieldAmount;
	FVoidDelegate OnNoShield;

	FVoidDelegate OnSetChangeHP;
	FVoidDelegate OnDie;






};
