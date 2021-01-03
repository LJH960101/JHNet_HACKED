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

	// �׾��� �� �ൿ�� ó���ϴ� �Լ�
	virtual void DieProcess();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

public:
	// �����ʿ��� ���� ������ ȣ��Ǵ� �Լ�.
	// �ʱ�ȭ�� ���⼭ �ϱ� �ٶ��ϴ�.
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
