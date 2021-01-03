// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Character.h"
#include "InGame/AI/HACKED_AI.h"
#include "InGame/Network/Component/NetworkBaseCP.h"
#include "SelfDestructAI.generated.h"

UCLASS()
class HACKED_API ASelfDestructAI : public AHACKED_AI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASelfDestructAI();

	class USD_AI_AnimInstance* SD_AI_Anim;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 죽었을 때 행동을 처리하는 함수
	virtual void DieProcess();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

public:
	// 스포너에서 스폰 됬을때 호출되는 함수.
	// 초기화를 여기서 하길 바랍니다.
	virtual void OnSpawn();

public:
	RPC_FUNCTION(ASelfDestructAI, SelfDestruct);
	void SelfDestruct();

	void SelfDestructDamaging();

	void SelfDestructDestroy();

private:
	void ResetAI();

	FTimerHandle SelfDestructTimerHandle;
	

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = SelfDestruct,
		Meta = (AllowPrivateAccess = true))
		bool bIsSelfDestruct;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfDestruct,
		Meta = (AllowPrivateAccess = true))
		float _destructRange;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SelfDestruct,
		Meta = (AllowPrivateAccess = true))
		float _destructDamage;

};
