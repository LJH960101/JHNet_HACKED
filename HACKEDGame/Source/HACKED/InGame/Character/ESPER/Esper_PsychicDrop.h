// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "HACKED_ESPER.h"
#include "../HACKEDCharacter.h"
#include "GameFramework/Actor.h"
#include "Esper_PsychicDrop.generated.h"

UCLASS()
class HACKED_API AEsper_PsychicDrop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEsper_PsychicDrop();

	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void PlaySkillM2Sound();

private:

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* PS_PsychicDropParticle;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* PsychicDropSphere;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* PsychicDropProjectile;

	UPROPERTY(VisibleAnywhere)
	UParticleSystem* PS_PsychicDropExplosion;

	UPROPERTY(VisibleAnywhere)
	UParticleSystem* PS_PsychicDropStun;


	FTimerHandle PsychicDropExpTime;


	float _pddamage = 0.0f;
	float _pdrange = 0.0f;

	AHACKED_ESPER* owner;
	AHACKEDCharacter* bigowner;
	class AHACKEDInGameManager* InGameManager;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetOwnerToEsper(AHACKED_ESPER* OwningActor);

	UFUNCTION(BlueprintCallable)
	void SetExplosionDamage(float damage);

	UFUNCTION(BlueprintCallable)
	void SetExplosionRange(float range);

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float speed);

	UFUNCTION(BlueprintCallable)
	void PsychicDropExplosion();

private:
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
