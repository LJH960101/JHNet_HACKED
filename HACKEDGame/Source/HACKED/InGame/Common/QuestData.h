// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "HACKED.h"
#include "Engine/DataTable.h"
#include "QuestData.generated.h"


// 서브 퀘스트를 묶는 상위 퀘스트
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

// 단일 퀘스트
// 모든 단일 퀘스트가 완수되면 상위 퀘스트가 완수됩니다.
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