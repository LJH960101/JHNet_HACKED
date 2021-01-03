// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Barrier.generated.h"

DECLARE_MULTICAST_DELEGATE(FVoidNotifyDelegate);

UCLASS()
class HACKED_API ABarrier : public AActor
{
	GENERATED_BODY()
	
public:	
	ABarrier();

protected:
	virtual void BeginPlay() override;

public:	
	//virtual void Tick(float DeltaTime) override;
	FORCEINLINE void SetBarrierSize(float _size) { barrierSize = _size; }
	FORCEINLINE void SetLifeTime(float _time) { lifeTime = _time; }
	void StopBarrier();

	FVoidNotifyDelegate OnBarrierStop;

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* generator;
	UPROPERTY(EditAnywhere)
	USphereComponent* collision;
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* barrierEffectComp;

	UMaterialInstanceDynamic* matInstance;
	FTimerHandle timer;
	//몇초동안 작동하는가
	UPROPERTY(EditAnywhere)
	float lifeTime;
	//베리어 사이즈
	UPROPERTY(EditAnywhere)
	float barrierSize;
	
	UFUNCTION()
	void OnBarrierHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
