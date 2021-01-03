// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "CoreBattery.generated.h"

UCLASS()
class HACKED_API ACoreBattery : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoreBattery();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	class UNetworkBaseCP* NetBaseCP;

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartHacking();

	virtual void Tick(float DeltaTime) override;

	RPC_FUNCTION(ACoreBattery, OPEN);
	void OPEN();

	FORCEINLINE UStaticMeshComponent* GetBase() { return base; }
	FORCEINLINE UStaticMeshComponent* GetTop() { return top; }
private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* collision;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* base;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* top;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* wandukong;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* coreEffectComp;
	UPROPERTY(EditAnywhere)
	float openingTime = 3.0f;
	UPROPERTY(VisibleAnywhere)
	float rot=0.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsOpening = false;
	UPROPERTY(VisibleAnywhere)
	bool bIsDone = false;
	
	bool doOnce = false;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
