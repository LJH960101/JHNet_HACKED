// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "MapChanger.generated.h"

UCLASS()
class HACKED_API AMapChanger : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapChanger();

protected:
	virtual void BeginPlay() override;

	/*UFUNCTION(BlueprintImplementableEvent, Category = "MapChanger")
	void OnLoad();*/

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEsperEnter();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCrusherEnter();

	virtual void Tick(float DeltaTime) override;
	////로드 할 레벨 이름
	//UPROPERTY(EditAnywhere)	
	//FName LevelToLoad;
	////언로드 할 레벨 이름
	//UPROPERTY(EditAnywhere)
	//FName LevelToUnload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* EsperNewStartPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* CrusherNewStartPoint;

	class AHACKEDInGameManager* HACKEDInGameManager;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent * OverlapVolume;

protected:
	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
