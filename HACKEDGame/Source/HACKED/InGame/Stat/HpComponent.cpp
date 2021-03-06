// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#include "HpComponent.h"

// Sets default values for this component's properties
UHpComponent::UHpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ..
	_hp = 0.0f;
	shieldCheck = false;
	_shield = 0.0f;
}

void UHpComponent::SetToMaxHP()
{
	_hp = _maxHp;
	OnSetChangeHP.Broadcast();
}

void UHpComponent::SetHP(float hp)
{
	_hp = FMath::Clamp(hp, 0.0f, _maxHp);
}

void UHpComponent::SetShield(float shield)
{
	_shield = FMath::Clamp(shield, 0.0f, _maxShield);
}

void UHpComponent::SetHPAndShield(float hp, float shield)
{
	SetHP(hp);
	SetShield(shield);
}

void UHpComponent::SetMaxHP(float maxHP)
{
	_maxHp = maxHP;
	_hp = _maxHp;
	OnSetChangeHP.Broadcast();
}

float UHpComponent::GetMaxHp()
{
	return _maxHp;
}

// Present Hp Get
float UHpComponent::GetHP()
{
	return _hp;
}

float UHpComponent::TakeDamage(float damage)
{
	if (shieldCheck && _shield <= 0.0f) {
		shieldCheck = false;
	}

	if (shieldCheck)
	{
		_shield = FMath::Clamp(_shield - damage, 0.0f, _maxShield);
		OnSetChangeShieldAmount.Broadcast();
		if (_shield <= 0)
		{
			shieldCheck = false;
			OnNoShield.Broadcast();
		}
		return _shield;
	}

	else if (!shieldCheck)
	{
		_hp = FMath::Clamp(_hp - damage, 0.0f, _maxHp);
		OnSetChangeHP.Broadcast();
		if (_hp <= 0)
			OnDie.Broadcast();
	}

	return _hp;
}

void UHpComponent::SetMaxShield(float maxShield)
{
	_maxShield = maxShield;
	_shield = 0.0f;
	OnSetChangeShieldAmount.Broadcast();
}


void UHpComponent::SetShieldAmount(float setShield)
{
	_shield = setShield;
}

float UHpComponent::GetMaxShield()
{
	return _maxShield;
}

float UHpComponent::GetShieldAmount()
{
	return _shield;
}



