// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "GameFramework/Actor.h"
#include "MachineGunBullet.generated.h"

UCLASS()
class HACKED_API AMachineGunBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMachineGunBullet();

private:
	bool onHit;

	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* PC_BulletParticle;

	UPROPERTY()
		UParticleSystem* PS_BulletHit;

	UPROPERTY(VisibleAnywhere)
		USphereComponent* BulletSphere;

	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* BulletProjectile;

	float _bulletDamage = 0.0f;

	AMilitaryBossAI* owner;

	UPROPERTY(VisibleAnywhere)
	USoundBase* SB_BulletHitGround;

	UPROPERTY(VisibleAnywhere)
	USoundBase* SB_BulletHitPlayer;

	UPROPERTY(VisibleAnywhere)
	USoundBase* SB_BulletHitBarrier;


public:
	UFUNCTION(BlueprintCallable)
		void SetOwnerToMB(AMilitaryBossAI* OwningActor);

	UFUNCTION(BlueprintCallable)
		void SetBulletShotDamage(float damage);

	UFUNCTION(BlueprintCallable)
		void SetBulletSpeed(float speed);

private:
	UFUNCTION()
		void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
