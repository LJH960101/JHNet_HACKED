// Fill out your copyright notice in the Description page of Project Settings.
#include "QuestManager.h"
#include "InGame/UI/WGQuest.h"

// Sets default values
AQuestManager::AQuestManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// ���� Ŭ���� �ʱ�ȭ
	static ConstructorHelpers::FClassFinder<UWGQuest> questWidget(TEXT("WidgetBlueprint'/Game/Blueprint/UI/InGame/Common/Quest/WG_Quest.WG_Quest_C'"));
	if (questWidget.Succeeded()) {
		WG_Quest_Class = questWidget.Class;
	}

	// ��������Ʈ ������ ���̺� �ʱ�ȭ
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MAINQUEST_DATA(TEXT("DataTable'/Game/Data/DT_MainQuest.DT_MainQuest'"));
	CHECK(DT_MAINQUEST_DATA.Succeeded());
	_mainQuestDataTable = DT_MAINQUEST_DATA.Object;
	CHECK(_mainQuestDataTable->GetRowMap().Num() > 0);

	// ��������Ʈ ������ ���̺� �ʱ�ȭ
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_SUBQUEST_DATA(TEXT("DataTable'/Game/Data/DT_SubQuest.DT_SubQuest'"));
	CHECK(DT_SUBQUEST_DATA.Succeeded());
	_subQuestDataTable = DT_SUBQUEST_DATA.Object;
	CHECK(_subQuestDataTable->GetRowMap().Num() > 0);
}

void AQuestManager::_RenewalWidget()
{
	WG_Quest->RenewalWidget(GetCurrentQuestDatas());
}

void AQuestManager::_CheckFinish()
{
	// ��� ����Ʈ�� ���¸� üũ�غ���.
	for (auto i : _currentSubQuests) {
		// ������ �ʾҴٸ� ����
		if (!_subQuestProgresses.Contains(i.subQuestCode) || _subQuestProgresses[i.subQuestCode] < MAX_PROGRESS) {
			return;
		}
	}

	// ��� ����Ʈ�� ������!!
	_mainQuestProgresses.Emplace(_currentMainQuest.mainQuestCode, true);
	_currentSubQuests.Empty();
}

void AQuestManager::AddProgressToSubQuest(FString subQuestCode, float progress)
{
	// ���� ���������� ã�´�.
	for (auto i : _currentSubQuests) {
		if (i.subQuestCode == subQuestCode) {
			// ���� �������̶�� ���൵�� �߰����ش�.
			_subQuestProgresses.Emplace(subQuestCode, _subQuestProgresses[subQuestCode] + progress);
			break;
		}
	}
	_CheckFinish();
	_RenewalWidget();
}

void AQuestManager::CompleteSubQuest(FString subQuestCode)
{
	AddProgressToSubQuest(subQuestCode, MAX_PROGRESS * 10.0f);
}

void AQuestManager::StartQuest(FString mainQuestCode)
{
	// �ڵ�� ��ġ�ϴ� ��������Ʈ �ڵ带 ã�´�.
	int rowNum = _mainQuestDataTable->GetRowMap().Num();
	for (int i_mainQuest = 1; i_mainQuest <= rowNum; ++i_mainQuest) {
		FMainQuest* mainQuestData = _mainQuestDataTable->FindRow<FMainQuest>(*FString::FromInt(i_mainQuest), TEXT(""));
		if (mainQuestData == nullptr) {
			LOG_S(Error);
			continue;
		}
		// �ش� ���� ����Ʈ �ڵ带 ã�Ҵ�!
		if (mainQuestData->mainQuestCode == mainQuestCode) {
			//���� ����Ʈ �ڵ带 �Ľ��Ѵ�.

			TArray<FString> paredSubQuestCode;

			// ���� ����Ʈ�� ���ٸ� ����.
			if (mainQuestData->subQuests.ParseIntoArray(paredSubQuestCode, TEXT(","), true) == 0) {
				LOG(Error, "This mainQuest dosen't have subquest... %s", *mainQuestCode);
				return;
			}

			// ���� ���¸� �����Ѵ�.
			_mainQuestProgresses.Emplace(mainQuestData->mainQuestCode, false);
			_currentMainQuest = *mainQuestData;
			_currentSubQuests.Empty();

			// �ش��ϴ� ���� ����Ʈ�� ������ ã�� �־��ش�.
			rowNum = _subQuestDataTable->GetRowMap().Num();
			for (int j_splitedSubQuest = 0; j_splitedSubQuest < paredSubQuestCode.Num(); ++j_splitedSubQuest) {
				for (int k_subQuest= 1; k_subQuest <= rowNum; ++k_subQuest) {
					FSubQuest* subQuestData = _subQuestDataTable->FindRow<FSubQuest>(*FString::FromInt(k_subQuest), TEXT(""));
					if (subQuestData == nullptr) {
						LOG_S(Error);
						continue;
					}
					if (subQuestData->subQuestCode == paredSubQuestCode[j_splitedSubQuest]) {
						// �Է�!
						_currentSubQuests.Add(*subQuestData);
						_subQuestProgresses.Emplace(subQuestData->subQuestCode, 0.0f);
					}
				}
			}

			_RenewalWidget();
			return;
		}
	}
	LOG(Error, "Not exist mainQuest... %s", *mainQuestCode);
}

bool AQuestManager::GetMainQuestProgress(FString mainQuestCode)
{
	if (!_mainQuestProgresses.Contains(mainQuestCode)) return false;
	else return _mainQuestProgresses[mainQuestCode];
}

float AQuestManager::GetSubQuestProgress(FString subQuestCode)
{
	if (_subQuestProgresses.Contains(subQuestCode)) return 0.0f;
	else return _subQuestProgresses[subQuestCode];
}

FString AQuestManager::GetCurrentQuestDatas()
{
	if (_currentSubQuests.Num() == 0) return FString(TEXT(""));

	FString retval = _currentMainQuest.title + TEXT(":");
	for (int i = 0; i < _currentSubQuests.Num(); ++i) {
		FSubQuest& subQuest = _currentSubQuests[i];
		int progress = (int)(_subQuestProgresses[subQuest.subQuestCode] + 0.5f);
		retval = retval + subQuest.title + TEXT("%") +
			FString::FromInt(progress);
		// �������� �ƴ϶�� &�� ���δ�.
		if (i != _currentSubQuests.Num() - 1)
			retval = retval + TEXT("&");
	}
	return retval;
}

void AQuestManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WG_Quest = CreateWidget<UWGQuest>(GetWorld(), WG_Quest_Class);
	WG_Quest->AddToViewport(1);
}

void AQuestManager::Destroyed()
{
	Super::Destroyed();
	if (WG_Quest) {
		WG_Quest->RemoveFromParent();
		WG_Quest = nullptr;
	}
}
