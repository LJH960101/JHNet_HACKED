// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "WaveData.h"
#include "WaveSystemLimiter.generated.h"

UCLASS()
class HACKED_API AWaveSystemLimiter : public AActor
{
	GENERATED_BODY()
	
public:	
	// �� AI�� �ִ� ��ġ+���� ������ �����մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WaveSystemLimiter")
	TMap<EHackedAIType, int> maxAiNumbs;

	// Sets default values for this actor's properties
	AWaveSystemLimiter();
	virtual void PostInitializeComponents() override;

	// �� á���� üũ�մϴ�.
	UFUNCTION(BlueprintPure, Category = "WaveSystemLimiter")
	bool IsFull(EHackedAIType type);

	// �� á���� üũ�մϴ�.
	UFUNCTION(BlueprintPure, Category = "WaveSystemLimiter")
	bool IsSomethingFull();

	// ������ ���� �ش� Ÿ���� ������Ʈ�� �޽��ϴ�.
	UFUNCTION(BlueprintCallable, Category = "Empty Zone")
	TArray<AActor*> GetExistObjects(EHackedAIType type);

	// ������ ���� �ش� Ÿ���� ������Ʈ�� ���� ���մϴ�.
	UFUNCTION(BlueprintPure, Category = "Empty Zone")
	int GetExistObjectNum(EHackedAIType type);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void _AddActor(AActor* actor);
	void _RemoveActor(AActor* actor);

	class AWaveSystem* _waveSystem;

	TMap<EHackedAIType, TArray<AActor*>> _overlapActors;
	TMap<EHackedAIType, UClass*> _spawnClasses;
	bool _bOnStopWave;
	class AHACKEDInGameManager* HACKEDInGameManager;
};
