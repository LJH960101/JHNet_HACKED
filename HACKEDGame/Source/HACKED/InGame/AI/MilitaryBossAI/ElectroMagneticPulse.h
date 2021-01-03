// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "InGame/Interface/HACKED_Enemy.h"
#include "InGame/AI/MilitaryBossAI/MilitaryBossAI.h"
#include "GameFramework/Actor.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "ElectroMagneticPulse.generated.h"

UCLASS()
class HACKED_API AElectroMagneticPulse : public AActor, public IHACKED_Enemy
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Network)
	class UNetworkBaseCP* NetBaseCP;

	// Sets default values for this actor's properties
	AElectroMagneticPulse();

	UFUNCTION(BlueprintImplementableEvent, Category = Explosion)
	void OnExplosion();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Boss)
	AMilitaryBossAI* MilitaryBoss;

private:

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ElectroMagnetPulse;

	UPROPERTY(VisibleAnywhere)
		UParticleSystem* PS_EMPExplosion;


private:
	UPROPERTY(EditAnywhere, Category = EMPDamage, Meta = (AllowPrivateAccess = true))
		float _empDamage;

private:

	UPROPERTY(VisibleAnywhere, Category = Stat)
		class UHpComponent* HpComponent;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	RPC_FUNCTION(AElectroMagneticPulse, EMPShock)
	UFUNCTION()
		void EMPShock();

public:
	UFUNCTION()
		void SetOutline(bool isOutLine);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PostInitializeComponents() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
