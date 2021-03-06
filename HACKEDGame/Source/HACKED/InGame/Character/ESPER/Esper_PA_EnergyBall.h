// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "HACKED_ESPER.h"
#include "../HACKEDCharacter.h"
#include "GameFramework/Actor.h"
#include "Esper_PA_EnergyBall.generated.h"

UCLASS()
class HACKED_API AEsper_PA_EnergyBall : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* PAParticle;

	UPROPERTY(VisibleAnywhere)
		USphereComponent* primaryAttackDamageComponent;

	UPROPERTY(VisibleAnywhere)
		USphereComponent* SPDamageComponent;


	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
		UParticleSystem* PAHitParticle;

	UPROPERTY()
		UParticleSystem* PASplashParticle;

	UPROPERTY()
		class USoundWave* PrimayHitSound;

	float _damage = 0.0f;
	float _spdamage = 0.0f;
	float _sprange = 0.0f;
	AHACKED_ESPER* owner;
	AHACKEDCharacter* bigowner;

public:	
	// Sets default values for this actor's properties
	AEsper_PA_EnergyBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 폭팔할때 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Energy Ball")
	void OnExplosion();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable)
	void SetOwnerToEsper(AHACKED_ESPER* OwningActor);

	UFUNCTION(BlueprintCallable)
	void SetprimaryAttackDamage(float damage);

	UFUNCTION(BlueprintCallable)
	void SetSPDamage(float spdamage);

	UFUNCTION(BlueprintCallable)
	void SetSPRange(float sprange);

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float speed);
	USphereComponent* GetSphere();

private:
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
