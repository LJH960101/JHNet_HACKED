// Copyright��2019 by ������ī��. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Engine/DataTable.h"
#include "QuestData.generated.h"


// ���� ����Ʈ�� ���� ���� ����Ʈ
USTRUCT(BlueprintType)
struct FMainQuest : public FTableRowBase
{
	GENERATED_BODY()

public:
	// MainQuest Code
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString mainQuestCode;

	// Quest Title
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString title;

	// Sub Quest List
	// Split by ,
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString subQuests;
};

// ���� ����Ʈ
// ��� ���� ����Ʈ�� �ϼ��Ǹ� ���� ����Ʈ�� �ϼ��˴ϴ�.
USTRUCT(BlueprintType)
struct FSubQuest : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Sub Quest Code
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString subQuestCode;

	// Sub Quest Title
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString title;
};