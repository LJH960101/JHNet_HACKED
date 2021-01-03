// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "SubtitleData.h"
#include "SubtitleMaker.generated.h"

UCLASS()
class HACKED_API ASubtitleMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASubtitleMaker();

protected:
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

public:	
	// �ڵ忡 �´� �ڸ��� �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void ShowSubtitleByCode(FString subtitleCode);

	// ���ڷ� ���� �ڸ��� �ð���ŭ �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void ShowSubtitle(FString subtitle, float time);

	// ������ �ڸ��� �����մϴ�.
	void StopSubtitle();

private:
	UPROPERTY()
	TSubclassOf<class UWGSubtitle> WG_Subtitle_Class;
	UPROPERTY()
	class UWGSubtitle* WG_Subtitle = nullptr;
	class UDataTable* _subtitleDataTable;
};
