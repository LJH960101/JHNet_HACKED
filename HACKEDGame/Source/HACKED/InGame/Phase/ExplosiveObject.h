// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "ExplosiveObject.generated.h"

UCLASS()
class HACKED_API AExplosiveObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AExplosiveObject();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	class UNetworkBaseCP* NetBaseCP;

protected:
	virtual void BeginPlay() override;

public:	
	FORCEINLINE void SetCanExplosive(bool canBeExplode) { bCanExplode = canBeExplode; }
	FORCEINLINE bool GetIsExploded() const { return isExploded; }
	FORCEINLINE void SetOutline(bool isOutlined) { explosiveMesh->SetRenderCustomDepth(isOutlined); }
	void Explode(AActor* explodeBy);

	RPC_FUNCTION(AExplosiveObject, RPCExplode)
	void RPCExplode();

private:
	class AHACKEDInGameManager* HACKEDInGameManager;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* explosiveMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* collision;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* explosionDamageSphere;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* explosionEffectComp;

	UPROPERTY(VisibleAnywhere)
	USoundWave* explosionSound;
	UPROPERTY(VisibleAnywhere)
	USoundAttenuation* explosionAttenuation;

	UPROPERTY(EditAnywhere)
	float damage=20.0f;

	UPROPERTY(EditAnywhere)
	float damageRadius = 300.0f;

	bool doOnce = false;

	bool bCanExplode = false;

	bool isExploded = false;
	
	UPROPERTY(VisibleAnywhere)
	int32 _hitCount = 0;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnFinished(UParticleSystemComponent* PSystem);
};
