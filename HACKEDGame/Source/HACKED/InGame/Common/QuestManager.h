// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HACKED.h"
#include "GameFramework/Actor.h"
#include "QuestData.h"
#include "QuestManager.generated.h"

UCLASS()
class HACKED_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AQuestManager();

	// 현재 진행중인 퀘스트중에서 해당 코드를 찾아 progress를 늘려줍니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddProgressToSubQuest(FString subQuestCode, float progress);

	// 현재 진행중인 퀘스트중에서 해당 코드를 찾아 progress를 최대로 늘려줍니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void CompleteSubQuest(FString subQuestCode);

	// 퀘스트를 시작합니다.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuest(FString mainQuestCode);

	// 메인 퀘스트가 끝났는지 확인합니다.
	UFUNCTION(BlueprintPure, Category = "Quest")
	bool GetMainQuestProgress(FString mainQuestCode);

	// 서브 퀘스트의 진행도를 확인합니다.
	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetSubQuestProgress(FString subQuestCode);

	// 퀘스트 데이터를 String으로 뭉쳐서 보냅니다.
	// 위젯에서 사용합니다.
	// 형식 : "제목:부제목%진행도&부제목%진행도"
	FString GetCurrentQuestDatas();
	// 안녕하세요:QUEST1%0&QUEST2%0&QUEST3%0

private:
	const float MAX_PROGRESS = 100.0f;

	UPROPERTY()
	TSubclassOf<class UWGQuest> WG_Quest_Class;
	UPROPERTY()
	class UWGQuest* WG_Quest = nullptr;
	class UDataTable* _mainQuestDataTable;
	class UDataTable* _subQuestDataTable;

	// 모든 메인퀘스트들의 완료사항
	TMap<FString, bool> _mainQuestProgresses;

	// 모든 서브퀘스트들의 진행도
	TMap<FString, float> _subQuestProgresses;
	
	// 현재 메인퀘스트
	FMainQuest _currentMainQuest;

	// 현재 서브퀘스트들
	// 이 배열이 비었으면 퀘스트가 끝났음을 감지합니다.
	TArray<FSubQuest> _currentSubQuests;

	// 위젯을 갱신합니다.
	void _RenewalWidget();

	// 모든 퀘스트가 끝났는지 확인합니다.
	void _CheckFinish();
	

protected:
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
};
