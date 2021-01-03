// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/AI/MilitaryNamedAI/MilitaryNamedAI.h"
#include "GameFramework/Actor.h"
#include "PhotonShot.generated.h"

UCLASS()
class HACKED_API APhotonShot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhotonShot();

private:
	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* PC_PhotonShotParticle;

	UPROPERTY(VisibleAnywhere)
		USphereComponent* PhotonShotSphere;

	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* PhotonShotProjectile;

	UPROPERTY()
		UParticleSystem* PS_PhotonShotExplosion;

	UPROPERTY(VisibleAnywhere)
		USoundBase* SB_PhotonHit;

	float _psDamage = 0.0f;
	float _psRange = 0.0f;

	AMilitaryNamedAI* owner;
	AHACKED_AI* bigowner;

public:
	UFUNCTION(BlueprintCallable)
	void SetOwnerToMN(AMilitaryNamedAI* OwningActor);

	UFUNCTION(BlueprintCallable)
	void SetPhotonShotDamage(float damage);

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float speed);

	UFUNCTION(BlueprintCallable)
	void SetExplosionRange(float range);

private:
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
