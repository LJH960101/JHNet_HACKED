// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "HomingMissile.generated.h"

UCLASS()
class HACKED_API AHomingMissile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHomingMissile(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	class AActor* Target;

	class AMilitaryBossAI* MilitaryBossAI;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UCameraShake> CS_HomingMissile;

	class AHACKEDInGameManager* HACKEDInGameManager;

private:
	UPROPERTY(VisibleAnywhere)
		USoundBase* SB_MissileExplosion1;
	UPROPERTY(VisibleAnywhere)
		USoundBase* SB_MissileExplosion2;
	UPROPERTY(VisibleAnywhere)
		USoundBase* SB_MissileExplosion3;
	UPROPERTY(EditAnywhere)
		USoundAttenuation* SA_MissileExplosionAttenuation;

public:

	UFUNCTION()
	void DelayLogic(float time);

	UFUNCTION()
	void UpdateTargetPlayer();

	UFUNCTION()
	void SetMissileDamage(float damage);

private:
	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile ,Meta = (AllowPrivateAccess = true))
		bool _hasTargetPosition;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		bool _hasNoTarget;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		float _delayTimer;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		bool _hasFinishedDelay;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		float _lifeTimeCountTime;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		float _startMissileSpeed;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		float _missileDamage;

	UPROPERTY(VisibleInstanceOnly, Category = HomingMissile, Meta = (AllowPrivateAccess = true))
		bool _bCanDestroy;
	
	UFUNCTION()
	void SetMissileSpeed(float speed);

	UFUNCTION()
	void MissileExplosion();


private:

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& hitResult);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UBoxComponent* MissileCollision;

	//UPROPERTY(VisibleAnywhere)
	//	class USphereComponent* MissileCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* MissileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UProjectileMovementComponent* MissileProjectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystemComponent* PC_MissileTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystem* PS_MissileTrail;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystemComponent* PC_MissileBlast;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystem* PS_MissileBlast;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystem* PS_MissileExplosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystem* PS_MissileBarrierExplosion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UMaterialInterface* DC_MissileDecal;

};
