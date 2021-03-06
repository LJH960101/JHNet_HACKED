// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "InGame/Interface/HACKED_Enemy.h"
#include "Battery.generated.h"

UCLASS()
class HACKED_API ABattery : public AActor, public IHACKED_Enemy
{
	GENERATED_BODY()
	
public:	
	ABattery();

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents()override;

	virtual void Tick(float DeltaTime) override;
	void Die();
private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* pedestal;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UHpComponent* HpComponent;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* top;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* bottom;
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* beamComp;
	UPROPERTY(EditAnywhere)
	UParticleSystem* beamEndEffect;
	UPROPERTY(EditAnywhere)
	UParticleSystem* beamLoopEffect;

	float stackTime = 0.0f;
	float floatingHeight = 50.0f;
	FVector originalTopPos;
	void TopFloating();

	void Dying();
	bool isAlive;
	bool doOnce;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
