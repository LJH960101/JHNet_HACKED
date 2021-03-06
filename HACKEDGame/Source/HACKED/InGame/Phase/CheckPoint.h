// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CheckPoint.generated.h"

UCLASS()
class HACKED_API ACheckPoint : public ATriggerBox
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	ACheckPoint();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintPure)
	bool IsBothEntered();

	UFUNCTION(BlueprintPure)
	bool IsCrusherEntered();

	UFUNCTION(BlueprintPure)
	bool IsEsperEntered();

	UFUNCTION(BlueprintPure)
	bool IsAnyEntered();

	bool bIsCrusher;
	bool bIsEsper;
};
