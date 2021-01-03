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

	// ���� �������� ����Ʈ�߿��� �ش� �ڵ带 ã�� progress�� �÷��ݴϴ�.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddProgressToSubQuest(FString subQuestCode, float progress);

	// ���� �������� ����Ʈ�߿��� �ش� �ڵ带 ã�� progress�� �ִ�� �÷��ݴϴ�.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void CompleteSubQuest(FString subQuestCode);

	// ����Ʈ�� �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuest(FString mainQuestCode);

	// ���� ����Ʈ�� �������� Ȯ���մϴ�.
	UFUNCTION(BlueprintPure, Category = "Quest")
	bool GetMainQuestProgress(FString mainQuestCode);

	// ���� ����Ʈ�� ���൵�� Ȯ���մϴ�.
	UFUNCTION(BlueprintPure, Category = "Quest")
	float GetSubQuestProgress(FString subQuestCode);

	// ����Ʈ �����͸� String���� ���ļ� �����ϴ�.
	// �������� ����մϴ�.
	// ���� : "����:������%���൵&������%���൵"
	FString GetCurrentQuestDatas();
	// �ȳ��ϼ���:QUEST1%0&QUEST2%0&QUEST3%0

private:
	const float MAX_PROGRESS = 100.0f;

	UPROPERTY()
	TSubclassOf<class UWGQuest> WG_Quest_Class;
	UPROPERTY()
	class UWGQuest* WG_Quest = nullptr;
	class UDataTable* _mainQuestDataTable;
	class UDataTable* _subQuestDataTable;

	// ��� ��������Ʈ���� �Ϸ����
	TMap<FString, bool> _mainQuestProgresses;

	// ��� ��������Ʈ���� ���൵
	TMap<FString, float> _subQuestProgresses;
	
	// ���� ��������Ʈ
	FMainQuest _currentMainQuest;

	// ���� ��������Ʈ��
	// �� �迭�� ������� ����Ʈ�� �������� �����մϴ�.
	TArray<FSubQuest> _currentSubQuests;

	// ������ �����մϴ�.
	void _RenewalWidget();

	// ��� ����Ʈ�� �������� Ȯ���մϴ�.
	void _CheckFinish();
	

protected:
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
};
