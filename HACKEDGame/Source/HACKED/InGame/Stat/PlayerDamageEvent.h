// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "PlayerDamageEvent.generated.h"

USTRUCT(BlueprintType)
struct HACKED_API FPlayerDamageEvent : public FDamageEvent
{
	GENERATED_BODY()
	
public:
	FPlayerDamageEvent() {
		bCanEsperStack = false;
		bCanChargeUlt = false;
		bWithAim = false;
		bCanExplosionEsperStack = false;
		bCanHitAnimation = false;
		bCanRigidity = false;
	}

	// ������ ���� �� �ִ°�?
	bool bCanRigidity;

	// �ǰ� �ִϸ��̼��� ���� �� �ִ°�?
	bool bCanHitAnimation;

	// ������ ������ ���� �� �ִ°�? -> ��Ÿ ���÷��ÿ��� ����ؾ��մϴ�.
	bool bCanEsperStack;

	// ������ ������ ��Ʈ�� �� �ִ°�? -> ��ų���� ����ؾ��մϴ�.
	bool bCanExplosionEsperStack;

	// �ñر⸦ ��¡�Ҽ��ִ°�?
	bool bCanChargeUlt;

	// �������� ���缭 ����ϴ� ��ų�ΰ�? -> UI�� ���� ���� ������ ���ؼ� ����մϴ�.
	bool bWithAim;

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 3;

	virtual int32 GetTypeID() const override { return FPlayerDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (FPlayerDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID); };

	/** Simple API for common cases where we are happy to assume a single hit is expected, even though damage event may have multiple hits. */
	virtual void GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, struct FHitResult& OutHitInfo, FVector& OutImpulseDir) const override;
};
