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

	// 스턴을 먹일 수 있는가?
	bool bCanRigidity;

	// 피격 애니메이션을 먹일 수 있는가?
	bool bCanHitAnimation;

	// 에스퍼 스택을 쌓을 수 있는가? -> 평타 스플래시에서 사용해야합니다.
	bool bCanEsperStack;

	// 에스퍼 스택을 터트릴 수 있는가? -> 스킬에서 사용해야합니다.
	bool bCanExplosionEsperStack;

	// 궁극기를 차징할수있는가?
	bool bCanChargeUlt;

	// 에임으로 맞춰서 사용하는 스킬인가? -> UI에 에임 적중 연출을 위해서 사용합니다.
	bool bWithAim;

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 3;

	virtual int32 GetTypeID() const override { return FPlayerDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (FPlayerDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID); };

	/** Simple API for common cases where we are happy to assume a single hit is expected, even though damage event may have multiple hits. */
	virtual void GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, struct FHitResult& OutHitInfo, FVector& OutImpulseDir) const override;
};
