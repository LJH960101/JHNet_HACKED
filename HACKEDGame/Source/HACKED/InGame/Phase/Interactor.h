// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "Interactor.generated.h"

UCLASS()
class HACKED_API AInteractor : public AActor
{
	GENERATED_BODY()

public:
	AInteractor();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AActor* caller);
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		bool bIsInteracted = false;
	void SetAsInteracted() { bIsInteracted = true; }
private:
	UPROPERTY(EditAnywhere)
		class UWidgetComponent* interactionWG;
	void ShowWidget();
	void CloseWidget();
	UPROPERTY()
	bool bIsAnyEntered = false;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* box;
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
