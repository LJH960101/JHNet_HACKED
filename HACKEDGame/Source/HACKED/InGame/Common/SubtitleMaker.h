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
	// 코드에 맞는 자막을 송출합니다.
	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void ShowSubtitleByCode(FString subtitleCode);

	// 인자로 들어온 자막을 시간만큼 송출합니다.
	UFUNCTION(BlueprintCallable, Category = "Subtitle")
	void ShowSubtitle(FString subtitle, float time);

	// 강제로 자막을 종료합니다.
	void StopSubtitle();

private:
	UPROPERTY()
	TSubclassOf<class UWGSubtitle> WG_Subtitle_Class;
	UPROPERTY()
	class UWGSubtitle* WG_Subtitle = nullptr;
	class UDataTable* _subtitleDataTable;
};
