// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "MagneticField.generated.h"

DECLARE_MULTICAST_DELEGATE(FVoidNotifyDelegate);

UCLASS()
class HACKED_API AMagneticField : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagneticField();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	RPC_FUNCTION(AMagneticField, SetMagneticState, bool);
	void SetMagneticState(bool _isPushState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	class UNetworkBaseCP* NetBaseCP;

	FVoidNotifyDelegate OnMagFieldStop;

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* magField;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* magneticEffectComp;
	UPROPERTY(VisibleAnywhere)
	UParticleSystem* pullEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystem* pushEffect;
	//밀고 당기는 상태 판별용 변수
	UPROPERTY(EditAnywhere)
	bool isPushState=false;
	//밀고 당기는 거리
	UPROPERTY(EditAnywhere)
	float activeDistance=1000.0f;
	//움직일 속도
	UPROPERTY(EditAnywhere)
	float magForceSpeed=1000.0f;
	UPROPERTY(EditAnywhere)
	float selectTime=3.0f;
	UPROPERTY(EditAnywhere)
	float lifeTime = 5.0f;
	float currentSpeed = 0.0f;
	FTimerHandle selectTimer;

private:
	void Cancel();

	void PushOrPull(float DeltaTime);
};
